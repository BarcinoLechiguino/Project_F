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
		App->dialog->dialog_state = DialogState::TEXT_TYPING;
		App->dialog->current_text.clear();
	}
}

//Dialog System
DialogSystem::DialogSystem()
{
	current_dialog = nullptr;
	dialog_font = nullptr;
	font_size = 20;

	dialog_state = DialogState::NOT_ACTIVE;

	is_clicked = false;
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
	if (App->scene_manager->current_scene->scene_name == SCENES::GAMEPLAY_SCENE)
	{
		switch (dialog_state)
		{
		case DialogState::NOT_ACTIVE:

			break;

		case DialogState::SLIDING_IN:

			SlideIn();

			break;

		case DialogState::TEXT_TYPING:

			TypeText();

			break;

		case DialogState::ACTIVE:

			NextBubbleCheck();

			break;

		case DialogState::SLIDING_OUT:

			SlideOut();

			break;
		}
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

	delete(timer);

	if (current_dialog != nullptr)
		delete(current_dialog);

	while (!dialog_queue.empty())
	{
		dialog_queue.pop();
	}

	for (std::vector<Dialog*>::iterator dialog = dialogs.begin(); dialog != dialogs.end(); ++dialog)
	{
		(*dialog)->dialog_bubbles.clear();
	}

	dialogs.clear();
	App->scene_manager->gameplay_scene->HUD_dialog_text.clear();

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
				App->scene_manager->gameplay_scene->HUD_dialogs_background->is_interactible = true;
				App->scene_manager->gameplay_scene->HUD_dialogs_background->SetElementPosition((*dialog)->position);

				App->gui_manager->CreateSlideAnimation(App->scene_manager->gameplay_scene->HUD_dialogs_background, 1.0f, false, iPoint(current_dialog->position.x - 500, current_dialog->position.y), current_dialog->position);

				dialog_queue.push((*dialog));

				return;
			}
		}
	}
}

void DialogSystem::NextBubbleCheck()
{
	if (timer->Read() >= current_dialog->dialog_bubbles[current_dialog->current_bubble]->active_time || is_clicked)
	{
		timer->Start();
		current_dialog->NextBubble();
		is_clicked = false;
	}
}

void DialogSystem::EmptyText()
{
	for (std::vector<UI_Text*>::iterator text = App->scene_manager->gameplay_scene->HUD_dialog_text.begin() ; text != App->scene_manager->gameplay_scene->HUD_dialog_text.end() ; ++text)
	{
		(*text)->DeleteCurrentStringTex();
	}
}

void DialogSystem::DisableText()
{
	for (std::vector<UI_Text*>::iterator text = App->scene_manager->gameplay_scene->HUD_dialog_text.begin(); text != App->scene_manager->gameplay_scene->HUD_dialog_text.end(); ++text)
	{
		(*text)->is_visible = false;
	}
}

void DialogSystem::EnableText()
{
	for (std::vector<UI_Text*>::iterator text = App->scene_manager->gameplay_scene->HUD_dialog_text.begin(); text != App->scene_manager->gameplay_scene->HUD_dialog_text.end(); ++text)
	{
		(*text)->is_visible = true;
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
		App->gui_manager->CreateSlideAnimation(App->scene_manager->gameplay_scene->HUD_dialogs_background, 1.0f, false, current_dialog->position, iPoint(current_dialog->position.x - 500, current_dialog->position.y));
		DisableText();
		EmptyText();
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
	int i = 0;
	for (std::vector<std::string>::iterator text = current_dialog->dialog_bubbles[current_dialog->current_bubble]->text.begin(); text != current_dialog->dialog_bubbles[current_dialog->current_bubble]->text.end(); ++text)
	{
		App->scene_manager->gameplay_scene->HUD_dialog_text[i]->RefreshTextInput((*text).c_str());
		i++;
	}

	dialog_state = DialogState::ACTIVE;
	timer->Start();
}

void DialogSystem::SetTextPosition(iPoint position)
{
	int y_position = position.y + 14; //Magic

	for (std::vector<UI_Text*>::iterator text = App->scene_manager->gameplay_scene->HUD_dialog_text.begin(); text != App->scene_manager->gameplay_scene->HUD_dialog_text.end(); ++text)
	{
		(*text)->SetElementPosition(iPoint(position.x + 20, y_position));

		y_position += font_size;
	}
}

void DialogSystem::SlideIn()
{
	

	if (!App->scene_manager->gameplay_scene->HUD_dialogs_background->is_transitioning)
	{
		dialog_state = DialogState::TEXT_TYPING;
		EnableText();
		App->scene_manager->gameplay_scene->HUD_dialogs_background->SetElementPosition(current_dialog->position);
		SetTextPosition(current_dialog->position);
	}
}

void DialogSystem::SlideOut()
{
	if (!App->scene_manager->gameplay_scene->HUD_dialogs_background->is_transitioning)
	{
		EmptyText();
		DisableText();
		current_dialog = nullptr;
		App->scene_manager->gameplay_scene->HUD_dialogs_background->is_visible = false;
		dialog_state = DialogState::NOT_ACTIVE;
	}
}

bool DialogSystem::LoadDialog()
{
	timer = new Timer;

	pugi::xml_parse_result result = dialog_file.load_file("dialog.xml");

	if (!result)
	{
		LOG("Dialog file could ot be loaded");
		return result;
	}
	else
	{
		pugi::xml_node dialog_font_node = dialog_file.child("dialog").child("dialog_font");

		font_size = dialog_font_node.attribute("font_size").as_int();
		dialog_font = App->font->Load(dialog_font_node.attribute("font").as_string(), font_size);
		dialog_color = { (Uint8)dialog_font_node.attribute("r").as_uint(),(Uint8)dialog_font_node.attribute("g").as_uint(),(Uint8)dialog_font_node.attribute("b").as_uint(),(Uint8)dialog_font_node.attribute("a").as_uint() };

		for (pugi::xml_node tree = dialog_file.child("dialog").child("dialog_tree"); tree != nullptr; tree = tree.next_sibling("dialog_tree"))
		{
			Dialog* buffer = new Dialog;

			buffer->dialog_id = tree.attribute("tree_id").as_int();
			buffer->position.x = tree.attribute("position_x").as_int();
			buffer->position.y = tree.attribute("position_y").as_int();
			//LOG("Loaded tree with id %d", buffer->dialog_id);
			LoadTextBubbles(buffer, tree);
			dialogs.push_back(buffer);
			//LOG("last id %d", buffer->last_id);
		}
	}

	current_dialog = nullptr; //start first dialog

	SDL_Rect HUD_dialogs_back_size = { 11, 643, 414, 124 };
	App->scene_manager->gameplay_scene->HUD_dialogs_background = (UI_Image*)App->gui_manager->CreateImage(UI_ELEMENT::IMAGE, 30, 30, HUD_dialogs_back_size, false,true, false, App->scene_manager->gameplay_scene, nullptr);

	App->scene_manager->gameplay_scene->HUD_dialog_text.push_back((UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 30, 30, { 0,0,0,0 }, dialog_font, dialog_color, false, false ,false,nullptr, App->scene_manager->gameplay_scene->HUD_dialogs_background));
	App->scene_manager->gameplay_scene->HUD_dialog_text.push_back((UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 30, 30, { 0,0,0,0 }, dialog_font, dialog_color, false, false, false, nullptr, App->scene_manager->gameplay_scene->HUD_dialogs_background));
	App->scene_manager->gameplay_scene->HUD_dialog_text.push_back((UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 30, 30, { 0,0,0,0 }, dialog_font, dialog_color, false, false, false, nullptr, App->scene_manager->gameplay_scene->HUD_dialogs_background));
	App->scene_manager->gameplay_scene->HUD_dialog_text.push_back((UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 30, 30, { 0,0,0,0 }, dialog_font, dialog_color, false, false, false, nullptr, App->scene_manager->gameplay_scene->HUD_dialogs_background));
	App->scene_manager->gameplay_scene->HUD_dialog_text.push_back((UI_Text*)App->gui_manager->CreateText(UI_ELEMENT::TEXT, 30, 30, { 0,0,0,0 }, dialog_font, dialog_color, false, false, false, nullptr, App->scene_manager->gameplay_scene->HUD_dialogs_background));
	dialog_state = DialogState::NOT_ACTIVE;

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
			//LOG("Loaded bubble with text: %s", text.attribute("text").as_string());
		}
		dialog_tree->dialog_bubbles.push_back(buffer);
	}

	return true;
}