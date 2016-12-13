#include "cefclient/tch_add/TchWindowApi.h"
using namespace Tnelab;

std::map<unsigned long, TchWindowApi::TchWindowSettings*> TchWindowApi::map_settings_;
TchAppStartSettings TchWindowApi::StartSettings;
void TchWindowApi::ClearSettings(unsigned long settings_id)
{
	auto it = map_settings_.find(settings_id);
	if (it == map_settings_.end())
		return;
	TchWindowApi::map_settings_.erase(it++);
}
TchWindowApi::TchWindowSettings* TchWindowApi::GetSettings(unsigned long settings_id)
{
	auto it = map_settings_.find(settings_id);
	DCHECK(it != map_settings_.end());
	return it->second;
}

void TchWindowApi::SetSettings(unsigned long settings_id, TchWindowApi::TchWindowSettings* ptr_settings)
{
	auto it = map_settings_.find(settings_id);
	DCHECK(it == map_settings_.end());
	map_settings_.insert(std::pair<unsigned long, Tnelab::TchWindowApi::TchWindowSettings*>(settings_id, ptr_settings));
}