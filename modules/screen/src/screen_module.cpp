#include <nodec_modules/screen/screen_module.hpp>

using namespace nodec;

namespace nodec_modules {
namespace screen {

ScreenModule::ScreenModule():
	Screen("nodec_modules::screen::ScreenModule")
{

};

Vector2i ScreenModule::resolution() const noexcept
{
	return resolution_;
}
void ScreenModule::set_resolution(const Vector2i& resolution)
{
	on_submit_resolution_change.invoke(*this, resolution);
}

Vector2i ScreenModule::size() const noexcept
{
	return size_;
}
void ScreenModule::set_size(const nodec::Vector2i& size)
{
	on_submit_size_change.invoke(*this, size);
}

std::string ScreenModule::title() const noexcept
{
	return title_;
}

void ScreenModule::set_title(const std::string& title)
{
	on_submit_title_change.invoke(*this, title);
}


}
}