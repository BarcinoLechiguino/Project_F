#include "Application.h"
#include "DialogSystem.h"
#include "Log.h"
#include "Render.h"
#include "SceneManager.h"
#include "GuiManager.h"
#include "UI_Image.h"
#include "UI_Text.h"
#include "GameplayScene.h"

#include "Fonts.h"

//Dialog Bubble
DialogBubble::DialogBubble()
{
	bubble_id = 0;
	active_time = 0;
	text.push_back("I have nothing to say");
}

//Dialog
Dialog::Dialog()
{
	dialog_id = 0;
	current_bubble = 0;
	last_id = 0;
}

void Dialog::NextBubble() //Next bubble in dialog
{
	current_bubble++;

	if (current_bubble > last_id)
	{
		App->dialog->NextDialog();
		current_bubble = 0;
	}
	else
	{
		//App->dialog->text_texture = App->font->Print(dialog_bubbles[current_bubble]->text.c_str(), App->dialog->dialog_color, App->dialog->dialog_font);
		//App->dialog->LoadDialogTextures();

		App->dialog->dialog_state = DialogState::TEXT_TYPING;
		App->dialog->current_text.clear();
	}

}

//Dialog System
DialogSystem::DialogSystem()
{
	current_dialog = nullptr;
	dialog_font = nullptr;

	dialog_color = { 255,255,100,255 };
	font_size = 20;

	dialog_state = DialogState::NOT_ACTIVE;

	timer = new Timer;
}

bool DialogSystem::Awake(pugi::xml_node&)
{
	return true;
}

bool DialogSystem::Start()
{
	return true;
}

bool DialogSystem::PreUpddate()
{
	return true;
}

bool DialogSystem::Update(float dt)
{
	
	switch (dialog_state)
	{
		case DialogState::NOT_ACTIVE:

			break;

		case DialogState::SLIDING_IN:

			SlideIn();

			break;

		case DialogState::ACTIVE:
				
			NextBubbleCheck();

			break;

		case DialogState::TEXT_TYPING:

			TypeText();

			break;

		case DialogState::SLIDING_OUT:

			SlideOut();

			break;
	}

	return true;
}

bool DialogSystem::PostUpdate()
{
	return true;
}

bool DialogSystem::CleanUp()
{
	//TODO cleanup
	return true;
}

//This starts or queues a new dialog
void DialogSystem::StartDialog(int tree_id) 
{
	if (dialog_state != DialogState::NOT_ACTIVE) //Add dialog to queue
	{
		for (std::vector<Dialog*>::iterator dialog = dialogs.begin(); dialog != dialogs.end(); ++dialog)
		{
			//push this new dialog
			if ((*dialog)->dialog_id == tree_id)
			{
				dialog_queue.push((*dialog));
				return;
			}
		}
	}
	else //set dialog to current_dialog
	{
		for (std::vector<Dialog*>::iterator dialog = dialogs.begin(); dialog != dialogs.end(); ++dialog)
		{
			if ((*dialog)->dialog_id == tree_id)
			{
				current_dialog = (*dialog);
				current_dialog->current_bubble = 0; //set current bubble to first bubble
				current_text.clear();

				dialog_state = DialogState::SLIDING_IN;

				App->scene_manager->gameplay_scene->HUD_dialogs_background->is_visible = true;
				App->scene_manager->gameplay_scene->HUD_dialogs_background->is_interactible = false;
				App->scene_manager->gameplay_scene->HUD_dialogs_background->SetScreenPos((*dialog)->position);

				dialog_queue.push((*dialog));

				return;
			}
		}
	}
}

void DialogSystem::NextBubbleCheck()
{
	if (timer->Read() >= current_dialog->dialog_bubbles[current_dialog->current_bubble]->active_time)
	{
		timer->Start();
		current_dialog->NextBubble();
	}
}

void DialogSystem::EmptyText()
{
	for (std::vector<UI_Text*>::iterator text = App->scene_manager->gameplay_scene->HUD_dialog_text.begin() ; text != App->scene_manager->gameplay_scene->HUD_dialog_text.end() ; ++text)
	{
		(*text)->GetString()->clear();
	}
}

void DialogSystem::NextDialog()
{
	//end it for now
	dialog_queue.pop();

	if (dialog_queue.empty())
	{
		dialog_state = DialogState::SLIDING_OUT;
		//current_dialog = nullptr;
		App->scene_manager->gameplay_scene->HUD_dialogs_background->is_interactible = false;

	}
	else
	{
		current_dialog = dialog_queue.front();
		current_dialog->current_bubble = 0;
		//LoadDialogTextures();
		current_text.clear();
		dialog_state = DialogState::TEXT_TYPING;
	}
}

void DialogSystem::TypeText()
{

}

void DialogSystem::SlideIn()
{

}

void DialogSystem::SlideOut()
{

}

bool DialogSystem::LoadDialog()
{
	dialog_font = App->font->Load("fonts/Minecraftia-Regular.ttf", font_size);

	pugi::xml_parse_result result = dialog_file.load_file("dialog.xml");

	if (!result)
	{
		LOG("Dialog file could ot be loaded");
		return result;
	}
	else
	{
		for (pugi::xml_node tree = dialog_file.child("dialog").child("dialog_tree"); tree != nullptr; tree = tree.next_sibling("dialog_tree"))
		{
			Dialog* buffer = new Dialog;

			buffer->dialog_id = tree.attribute("tree_id").as_int();
			buffer->position.x = tree.attribute("position_x").as_int();
			buffer->position.y = tree.attribute("position_y").as_int();
			LOG("Loaded tree with id %d", buffer->dialog_id);
			LoadTextBubbles(buffer, tree);
			dialogs.push_back(buffer);
			LOG("last id %d", buffer->last_id);
		}
	}

	current_dialog = dialogs.front(); //start first dialog

	SDL_Rect HUD_dialogs_back_size = { 11, 643, 414, 124 };
	App->scene_manager->gameplay_scene->HUD_dialogs_background = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::IMAGE, 30, 30, HUD_dialogs_back_size, false, true, false, this, nullptr);

	App->scene_manager->gameplay_scene->HUD_dialog_text.push_back((UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 30, 30, { 0,0,0,0 }, dialog_font, dialog_color, false, false ,false, nullptr, App->scene_manager->gameplay_scene->HUD_dialogs_background));
	App->scene_manager->gameplay_scene->HUD_dialog_text.push_back((UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 30, 30, { 0,0,0,0 }, dialog_font, dialog_color, false, false, false, nullptr, App->scene_manager->gameplay_scene->HUD_dialogs_background));
	App->scene_manager->gameplay_scene->HUD_dialog_text.push_back((UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 30, 30, { 0,0,0,0 }, dialog_font, dialog_color, false, false, false, nullptr, App->scene_manager->gameplay_scene->HUD_dialogs_background));

	return result;
}

bool DialogSystem::LoadTextBubbles(Dialog* dialog_tree, pugi::xml_node tree)
{
	for (pugi::xml_node node = tree.child("dialog_bubble"); node != nullptr; node = node.next_sibling("dialog_bubble"))
	{
		DialogBubble* buffer = new DialogBubble;
		buffer->bubble_id, dialog_tree->last_id  = node.attribute("bubble_id").as_int(); //last_id will save last id looped
		buffer->active_time = node.attribute("active_time").as_int();
		buffer->text.clear(); //clear empty text

		for (pugi::xml_node text = node.child("text"); text != nullptr; text = text.next_sibling("text"))
		{
			buffer->text.push_back(text.attribute("text").as_string());
			LOG("Loaded bubble with text: %s", text.attribute("text").as_string());
		}
		dialog_tree->dialog_bubbles.push_back(buffer);
	}

	return true;
}