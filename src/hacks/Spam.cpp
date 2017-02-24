/*
 * Spam.cpp
 *
 *  Created on: Jan 21, 2017
 *      Author: nullifiedcat
 */

#include "Spam.h"
#include "../common.h"
#include "../sdk.h"
#include <pwd.h>

namespace hacks { namespace shared { namespace spam {

CatVar enabled(CV_SWITCH, "spam", "0", "Chat spam", NULL, "Enable Spam");
CatVar filename(CV_STRING, "spam_file", "spam.txt", "Spam file (~/.cathook/...)", NULL, "Spam file name. Each line should be no longer than 100 characters, file must be located in ~/.cathook folder");
CatVar newlines(CV_SWITCH, "spam_newlines", "1", "Prepend newlines to messages", NULL, "If enabled, several newlines will be added before each message");
CatCommand reload("spam_reload", []() {
	file.LoadFile(filename.GetString());
}, "Reload spam file");

TextFile file;
size_t index = 0;

auto init_func = []() {
	chat_stack::AddProvider([]() -> std::string {
		if (!enabled) return "";
		if (!file.LineCount()) return "";
		if (index >= file.LineCount()) index = 0;
		std::string spam = (newlines ? format("\n\n\n\n\n\n\n\n\n\n\n\n", file.Line(index)) : file.Line(index));
		ReplaceString(spam, "\\n", "\n");
		index++;
	});
}();

}}}
