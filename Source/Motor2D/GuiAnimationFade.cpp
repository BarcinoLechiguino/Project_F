#include "Application.h"
#include "Render.h"
#include "GuiManager.h"

#include "GuiElement.h"

#include "GuiAnimationFade.h"

GuiAnimationFade::GuiAnimationFade(GuiElement* element, float animation_duration, bool hide_on_completion, float start_alpha, float end_alpha) 
	: GuiAnimation(element, GUI_ANIMATION_TYPE::FADE,  animation_duration, hide_on_completion)
	, start_alpha(start_alpha)
	, end_alpha(end_alpha)
{
	InitFade();
}

GuiAnimationFade::~GuiAnimationFade()
{

}

void GuiAnimationFade::Start()
{

}

void GuiAnimationFade::StepAnimation()
{
	if (!fade_from_higher_to_lower)
	{
		current_animation_step_rate += GetAnimationStepRate(animation_duration);

		if (current_animation_step_rate > MAX_RATE)
		{
			to_delete = true;

			FinishAnimation();
		}
		else
		{
			ApplyFade();
		}
	}
	else
	{
		current_animation_step_rate -= GetAnimationStepRate(animation_duration);

		if (current_animation_step_rate < MIN_RATE)
		{
			to_delete = true;

			FinishAnimation();
		}
		else
		{
			ApplyFade();
		}
	}
}

void GuiAnimationFade::CleanUp()
{
	if (element->is_transitioning)
	{
		FinishAnimation();
	}
	
	return;
}

void GuiAnimationFade::FinishAnimation()
{
	element->is_transitioning = false;
	
	if (element_texture != nullptr)
	{
		SDL_SetTextureAlphaMod(element_texture, (Uint8)end_alpha);
	}
	
	if (hide_on_completion)
	{
		element->is_visible = false;
	}

	// Finishing the Childs
	for (int i = 0; i < textures_to_fade.size(); ++i)
	{
		SDL_SetTextureAlphaMod(textures_to_fade[i], (Uint8)end_alpha);
	}

	for (int i = 0; i < child_elements.size(); ++i)
	{
		child_elements[i]->is_transitioning = false;

		if (hide_on_completion)
		{
			child_elements[i]->is_visible = false;
		}
	}

	textures_to_fade.clear();
	child_elements.clear();
}

void GuiAnimationFade::ApplyFade()
{
	if (element_texture != nullptr)
	{
		if (end_alpha == 0.0f)
		{
			SDL_SetTextureAlphaMod(element_texture, (Uint8)(current_animation_step_rate * 255.0f));
		}
		else
		{
			SDL_SetTextureAlphaMod(element_texture, (Uint8)(current_animation_step_rate * end_alpha));
		}
		
	}

	for (int i = 0; i < textures_to_fade.size(); ++i)
	{
		if (end_alpha == 0.0f)
		{
			SDL_SetTextureAlphaMod(textures_to_fade[i], (Uint8)(current_animation_step_rate * 255.0f));
		}
		else
		{
			SDL_SetTextureAlphaMod(textures_to_fade[i], (Uint8)(current_animation_step_rate * end_alpha));
		}
	}
}

void GuiAnimationFade::InitFade()
{
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

	if (start_alpha > end_alpha)
	{
		fade_from_higher_to_lower = true;
		current_animation_step_rate = MAX_RATE;
	}
	else
	{
		fade_from_higher_to_lower = false;
	}

	GetAndInitializeTexturesToFade();
}

void GuiAnimationFade::GetAndInitializeTexturesToFade()
{	
	element->is_transitioning = true;
	element_texture = element->GetTexture();

	if (element_texture != nullptr)
	{
		SDL_SetTextureBlendMode(element_texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(element_texture, (Uint8)start_alpha);
	}
	
	// GET CHILDS' TEXTURES
	child_elements = App->gui_manager->GetElementChilds(element);

	SDL_Texture* child_texture;

	for (int i = 0; i < child_elements.size(); ++i)
	{
		child_texture = child_elements[i]->GetTexture();

		if (child_texture != nullptr)
		{
			child_elements[i]->is_transitioning = true;
			
			SDL_SetTextureBlendMode(child_texture, SDL_BLENDMODE_BLEND);		// Needed ?
			SDL_SetTextureAlphaMod(child_texture, (Uint8)start_alpha);
			
			textures_to_fade.push_back(child_texture);
		}
	}
}