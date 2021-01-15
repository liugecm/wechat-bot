#pragma once
#include <Windows.h>
#include <memory>
#include <arris/util/log_util.hpp>
#include <arris/wechat/offset_version.hpp>
#include <arris/wechat/hookmsg_mgr.hpp>
using namespace arris::util;
using namespace arris::wechat;

namespace arris {
namespace wechat{
	DWORD over_receive_msg_call_addr = get_wx_baseaddr() + static_cast<DWORD>(version::hook_common_offset::call_offset);
	DWORD ret_receive_msg_addr = get_wx_baseaddr() + static_cast<DWORD>(version::hook_common_offset::hook_point_offset) + 5;
	
	void __stdcall dispatch_hook_msg(DWORD r_eax) {
		__OutputDebugString(TEXT("eax:%x\n"),r_eax);
		std::unique_ptr<hookmsg_mgr_ex> ptr=std::make_unique<hookmsg_mgr_ex>(r_eax);;
		DWORD type = read_dword(NULL,r_eax+static_cast<DWORD>(version::hook_msg_type::hook_msg_type_offset));
		__OutputDebugString(TEXT("hook msg type:%d"),type);
		wx_msg st_msg;//not used
		ptr->run(type,st_msg);
	}
	__declspec(naked) void receive_wx_msg()
	{		__asm
		{
			pushad;
			pushfd;
			push eax;
			call dispatch_hook_msg;
			popfd;
			popad;
			//调用被覆盖的call
			call over_receive_msg_call_addr;
			//跳转到返回地址
			jmp ret_receive_msg_addr;
		}
	}
	void __stdcall hook() {
		DWORD hook_offset = get_wx_baseaddr() + static_cast<DWORD>(version::hook_common_offset::hook_point_offset);
		//ptr = 
		__OutputDebugString(TEXT("hook offset:%x\n"),hook_offset);
		hook_five_bytes(hook_offset,(DWORD)receive_wx_msg);
	}
}//namespace hook
}//namespace arris