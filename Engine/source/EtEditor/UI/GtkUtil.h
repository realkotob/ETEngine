#pragma once

#include <EtCore/Helper/InputManager.h>


namespace Gtk {


//---------------------------------
// make_managed
//
// Function that the GTKmm book suggests using but doesn't actually exist in the codebase
//
template<class T, class... T_Args>
auto make_managed(T_Args&&... args)
{
	return manage(new T(std::forward<T_Args>(args)...));
}


} // namespace Gtk



namespace GtkUtil {

	E_MouseButton GetButtonFromGtk(uint32 const buttonCode);
	E_KbdKey GetKeyFromGtk(uint32 const keyCode);

} // namepace GtkUtil
