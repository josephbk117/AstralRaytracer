#pragma region PVS STUDIO

// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma endregion

#pragma region IGFD LICENSE

/*
MIT License

Copyright (c) 2019-2020 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma endregion

#ifndef IMGUI_DEFINE_MATH_OPERATORS
	#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "WindowFramework/UI/FileDialog/ImGuiFileDialog.h"

#ifdef __cplusplus

	#pragma region Includes

	#include <cerrno>
	#include <cfloat>
	#include <cstdarg> // variadic
	#include <cstdio>
	#include <cstring> // stricmp / strcasecmp
	#include <ctime>
	#include <iomanip>
	#include <sstream>
	#include <sys/stat.h>
	#include <exception>
	#include <filesystem>

	#ifdef __EMSCRIPTEN__
		#include <emscripten.h>
	#endif // __EMSCRIPTEN__

	#ifdef _MSC_VER

		#define IGFD_DEBUG_BREAK  \
			if(IsDebuggerPresent()) \
			__debugbreak()
	#else
		#define IGFD_DEBUG_BREAK
	#endif

	#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || \
			defined(WIN64) || defined(_WIN64) || defined(_MSC_VER)
		#define _IGFD_WIN_
		#define stat    _stat
		#define stricmp _stricmp
		#include <cctype>
		#include <windows.h>
		#define PATH_SEP '\\'
		#ifndef PATH_MAX
			#define PATH_MAX 260
		#endif // PATH_MAX
	#elif defined(__linux__) || defined(__FreeBSD__) || defined(__DragonFly__) || \
			defined(__NetBSD__) || defined(__APPLE__) || defined(__EMSCRIPTEN__)
		#define _IGFD_UNIX_
		#define stricmp strcasecmp
		#include <sys/types.h>
		#define PATH_SEP '/'
	#endif // _IGFD_UNIX_

	#include "imgui.h"
	#include "imgui_internal.h"

	#include <algorithm>
	#include <cstdlib>
	#include <iostream>

	#pragma endregion

	#pragma region Common defines

	#ifdef USE_THUMBNAILS
		#ifndef DONT_DEFINE_AGAIN__STB_IMAGE_IMPLEMENTATION
			#ifndef STB_IMAGE_IMPLEMENTATION
				#define STB_IMAGE_IMPLEMENTATION
			#endif // STB_IMAGE_IMPLEMENTATION
		#endif   // DONT_DEFINE_AGAIN__STB_IMAGE_IMPLEMENTATION
		#include "stbimage/stb_image.h"
		#ifndef DONT_DEFINE_AGAIN__STB_IMAGE_RESIZE_IMPLEMENTATION
			#ifndef STB_IMAGE_RESIZE_IMPLEMENTATION
				#define STB_IMAGE_RESIZE_IMPLEMENTATION
			#endif // STB_IMAGE_RESIZE_IMPLEMENTATION
		#endif   // DONT_DEFINE_AGAIN__STB_IMAGE_RESIZE_IMPLEMENTATION
		#include "stbimage/stb_image_resize.h"
	#endif // USE_THUMBNAILS

	// float comparisons
	#ifndef IS_FLOAT_DIFFERENT
		#define IS_FLOAT_DIFFERENT(a, b) (fabs((a) - (b)) > FLT_EPSILON)
	#endif // IS_FLOAT_DIFFERENT
	#ifndef IS_FLOAT_EQUAL
		#define IS_FLOAT_EQUAL(a, b) (fabs((a) - (b)) < FLT_EPSILON)
	#endif // IS_FLOAT_EQUAL

	#pragma endregion

	#pragma region IGFD NAMESPACE

	#pragma region CUSTOMIZATION DEFINES

	///////////////////////////////
	// COMBOBOX
	///////////////////////////////
	#ifndef FILTER_COMBO_AUTO_SIZE
		#define FILTER_COMBO_AUTO_SIZE 1
	#endif // FILTER_COMBO_AUTO_SIZE
	#ifndef FILTER_COMBO_MIN_WIDTH
		#define FILTER_COMBO_MIN_WIDTH 150.0f
	#endif // FILTER_COMBO_MIN_WIDTH
	#ifndef IMGUI_BEGIN_COMBO
		#define IMGUI_BEGIN_COMBO ImGui::BeginCombo
	#endif // IMGUI_BEGIN_COMBO
	///////////////////////////////
	// BUTTON
	///////////////////////////////
	// for lets you define your button widget
	// if you have like me a special bi-color button
	#ifndef IMGUI_PATH_BUTTON
		#define IMGUI_PATH_BUTTON ImGui::Button
	#endif // IMGUI_PATH_BUTTON
	#ifndef IMGUI_BUTTON
		#define IMGUI_BUTTON ImGui::Button
	#endif // IMGUI_BUTTON
	///////////////////////////////
	// locales
	///////////////////////////////
	#ifndef createDirButtonString
		#define createDirButtonString "+"
	#endif // createDirButtonString
	#ifndef okButtonString
		#define okButtonString "OK"
	#endif // okButtonString
	#ifndef okButtonWidth
		#define okButtonWidth 0.0f
	#endif // okButtonWidth
	#ifndef cancelButtonString
		#define cancelButtonString "Cancel"
	#endif // cancelButtonString
	#ifndef cancelButtonWidth
		#define cancelButtonWidth 0.0f
	#endif // cancelButtonWidth
	#ifndef okCancelButtonAlignement
		#define okCancelButtonAlignement 0.0f
	#endif // okCancelButtonAlignement
	#ifndef invertOkAndCancelButtons
		// 0 => disabled, 1 => enabled
		#define invertOkAndCancelButtons 0
	#endif // invertOkAndCancelButtons
	#ifndef resetButtonString
		#define resetButtonString "R"
	#endif // resetButtonString
	#ifndef drivesButtonString
		#define drivesButtonString "Drives"
	#endif // drivesButtonString
	#ifndef editPathButtonString
		#define editPathButtonString "E"
	#endif // editPathButtonString
	#ifndef searchString
		#define searchString "Search :"
	#endif // searchString
	#ifndef dirEntryString
		#define dirEntryString "[Dir]"
	#endif // dirEntryString
	#ifndef linkEntryString
		#define linkEntryString "[Link]"
	#endif // linkEntryString
	#ifndef fileEntryString
		#define fileEntryString "[File]"
	#endif // fileEntryString
	#ifndef fileNameString
		#define fileNameString "File Name :"
	#endif // fileNameString
	#ifndef dirNameString
		#define dirNameString "Directory Path :"
	#endif // dirNameString
	#ifndef buttonResetSearchString
		#define buttonResetSearchString "Reset search"
	#endif // buttonResetSearchString
	#ifndef buttonDriveString
		#define buttonDriveString "Drives"
	#endif // buttonDriveString
	#ifndef buttonEditPathString
		#define buttonEditPathString "Edit path\nYou can also right click on path buttons"
	#endif // buttonEditPathString
	#ifndef buttonResetPathString
		#define buttonResetPathString "Reset to current directory"
	#endif // buttonResetPathString
	#ifndef buttonCreateDirString
		#define buttonCreateDirString "Create Directory"
	#endif // buttonCreateDirString
	#ifndef tableHeaderAscendingIcon
		#define tableHeaderAscendingIcon "A|"
	#endif // tableHeaderAscendingIcon
	#ifndef tableHeaderDescendingIcon
		#define tableHeaderDescendingIcon "D|"
	#endif // tableHeaderDescendingIcon
	#ifndef tableHeaderFileNameString
		#define tableHeaderFileNameString "File name"
	#endif // tableHeaderFileNameString
	#ifndef tableHeaderFileTypeString
		#define tableHeaderFileTypeString "Type"
	#endif // tableHeaderFileTypeString
	#ifndef tableHeaderFileSizeString
		#define tableHeaderFileSizeString "Size"
	#endif // tableHeaderFileSizeString
	#ifndef tableHeaderFileDateString
		#define tableHeaderFileDateString "Date"
	#endif // tableHeaderFileDateString
	#ifndef fileSizeBytes
		#define fileSizeBytes "o"
	#endif // fileSizeBytes
	#ifndef fileSizeKiloBytes
		#define fileSizeKiloBytes "Ko"
	#endif // fileSizeKiloBytes
	#ifndef fileSizeMegaBytes
		#define fileSizeMegaBytes "Mo"
	#endif // fileSizeMegaBytes
	#ifndef fileSizeGigaBytes
		#define fileSizeGigaBytes "Go"
	#endif // fileSizeGigaBytes
	#ifndef OverWriteDialogTitleString
		#define OverWriteDialogTitleString "The file Already Exist !"
	#endif // OverWriteDialogTitleString
	#ifndef OverWriteDialogMessageString
		#define OverWriteDialogMessageString "Would you like to OverWrite it ?"
	#endif // OverWriteDialogMessageString
	#ifndef OverWriteDialogConfirmButtonString
		#define OverWriteDialogConfirmButtonString "Confirm"
	#endif // OverWriteDialogConfirmButtonString
	#ifndef OverWriteDialogCancelButtonString
		#define OverWriteDialogCancelButtonString "Cancel"
	#endif // OverWriteDialogCancelButtonString
	#ifndef DateTimeFormat
		// see strftime functionin <ctime> for customize
		#define DateTimeFormat "%Y/%m/%d %H:%M"
	#endif // DateTimeFormat
	///////////////////////////////
	// THUMBNAILS
	///////////////////////////////
	#ifdef USE_THUMBNAILS
		#ifndef tableHeaderFileThumbnailsString
			#define tableHeaderFileThumbnailsString "Thumbnails"
		#endif // tableHeaderFileThumbnailsString
		#ifndef DisplayMode_FilesList_ButtonString
			#define DisplayMode_FilesList_ButtonString "FL"
		#endif // DisplayMode_FilesList_ButtonString
		#ifndef DisplayMode_FilesList_ButtonHelp
			#define DisplayMode_FilesList_ButtonHelp "File List"
		#endif // DisplayMode_FilesList_ButtonHelp
		#ifndef DisplayMode_ThumbailsList_ButtonString
			#define DisplayMode_ThumbailsList_ButtonString "TL"
		#endif // DisplayMode_ThumbailsList_ButtonString
		#ifndef DisplayMode_ThumbailsList_ButtonHelp
			#define DisplayMode_ThumbailsList_ButtonHelp "Thumbnails List"
		#endif // DisplayMode_ThumbailsList_ButtonHelp
		#ifndef DisplayMode_ThumbailsGrid_ButtonString
			#define DisplayMode_ThumbailsGrid_ButtonString "TG"
		#endif // DisplayMode_ThumbailsGrid_ButtonString
		#ifndef DisplayMode_ThumbailsGrid_ButtonHelp
			#define DisplayMode_ThumbailsGrid_ButtonHelp "Thumbnails Grid"
		#endif // DisplayMode_ThumbailsGrid_ButtonHelp
		#ifndef DisplayMode_ThumbailsList_ImageHeight
			#define DisplayMode_ThumbailsList_ImageHeight 32.0f
		#endif // DisplayMode_ThumbailsList_ImageHeight
		#ifndef IMGUI_RADIO_BUTTON
inline bool inRadioButton(const char* vLabel, bool vToggled)
{
	bool pressed= false;
	if(vToggled)
	{
		ImVec4 bua= ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
		ImVec4 te = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		ImGui::PushStyleColor(ImGuiCol_Button, te);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, te);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, te);
		ImGui::PushStyleColor(ImGuiCol_Text, bua);
	}
	pressed= IMGUI_BUTTON(vLabel);
	if(vToggled)
	{
		ImGui::PopStyleColor(4); //-V112
	}
	return pressed;
}

			#define IMGUI_RADIO_BUTTON inRadioButton
		#endif // IMGUI_RADIO_BUTTON
	#endif   // USE_THUMBNAILS
	///////////////////////////////
	// BOOKMARKS
	///////////////////////////////
	#ifdef USE_BOOKMARK
		#ifndef defaultBookmarkPaneWith
			#define defaultBookmarkPaneWith 150.0f
		#endif // defaultBookmarkPaneWith
		#ifndef bookmarksButtonString
			#define bookmarksButtonString "Bookmark"
		#endif // bookmarksButtonString
		#ifndef bookmarksButtonHelpString
			#define bookmarksButtonHelpString "Bookmark"
		#endif // bookmarksButtonHelpString
		#ifndef addBookmarkButtonString
			#define addBookmarkButtonString "+"
		#endif // addBookmarkButtonString
		#ifndef removeBookmarkButtonString
			#define removeBookmarkButtonString "-"
		#endif // removeBookmarkButtonString
		#ifndef IMGUI_TOGGLE_BUTTON
inline bool inToggleButton(const char* vLabel, bool* vToggled)
{
	bool pressed= false;

	if(vToggled && *vToggled)
	{
		ImVec4 bua= ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
		// ImVec4 buh = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
		// ImVec4 bu = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		ImVec4 te= ImGui::GetStyleColorVec4(ImGuiCol_Text);
		ImGui::PushStyleColor(ImGuiCol_Button, te);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, te);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, te);
		ImGui::PushStyleColor(ImGuiCol_Text, bua);
	}

	pressed= IMGUI_BUTTON(vLabel);

	if(vToggled && *vToggled)
	{
		ImGui::PopStyleColor(4); //-V112
	}

	if(vToggled && pressed)
	{
		*vToggled= !*vToggled;
	}

	return pressed;
}

			#define IMGUI_TOGGLE_BUTTON inToggleButton
		#endif // IMGUI_TOGGLE_BUTTON
	#endif   // USE_BOOKMARK

	#pragma endregion

	#pragma region INTERNAL

	#pragma region EXCEPTION

class IGFDException: public std::exception
{
	private:
		std::string m_Message;
	public:
		IGFDException(const std::string& vMessage): m_Message(vMessage) { }

		const char* what() { return m_Message.c_str(); }
};

	#pragma endregion

	#pragma region Utils

// https://github.com/ocornut/imgui/issues/1720
IGFD_API bool IGFD::Utils::ImSplitter(
		bool   split_vertically,
		float  thickness,
		float* size1,
		float* size2,
		float  min_size1,
		float  min_size2,
		float  splitter_long_axis_size
)
{
	using namespace ImGui;
	ImGuiContext& g     = *GImGui;
	ImGuiWindow*  window= g.CurrentWindow;
	ImGuiID       id    = window->GetID("##Splitter");
	ImRect        bb;

	ImVec2 sizeVal= split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1);
	bb.Min.x      = window->DC.CursorPos.x + sizeVal.x;
	bb.Min.y      = window->DC.CursorPos.y + sizeVal.y;

	ImVec2 splitterSize= split_vertically ? ImVec2(thickness, splitter_long_axis_size)
																				: ImVec2(splitter_long_axis_size, thickness);
	ImVec2 calcItemSize= CalcItemSize(splitterSize, 0.0f, 0.0f);
	bb.Max.x           = bb.Min.x + calcItemSize.x;
	bb.Max.y           = bb.Min.y + calcItemSize.y;
	return SplitterBehavior(
			bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 1.0f
	);
}

// Convert a wide Unicode string to an UTF8 string
IGFD_API std::string IGFD::Utils::UTF8Encode(const std::wstring& wstr)
{
	std::string res;
	#ifdef _IGFD_WIN_
	if(!wstr.empty())
	{
		int size_needed=
				WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		if(size_needed)
		{
			res= std::string(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &res[0], size_needed, NULL, NULL);
		}
	}
	#else
	// Suppress warnings from the compiler.
	(void)wstr;
	#endif // _IGFD_WIN_
	return res;
}

// Convert an UTF8 string to a wide Unicode String
IGFD_API std::wstring IGFD::Utils::UTF8Decode(const std::string& str)
{
	std::wstring res;
	#ifdef _IGFD_WIN_
	if(!str.empty())
	{
		int size_needed= MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		if(size_needed)
		{
			res= std::wstring(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &res[0], size_needed);
		}
	}
	#else
	// Suppress warnings from the compiler.
	(void)str;
	#endif // _IGFD_WIN_
	return res;
}

IGFD_API bool IGFD::Utils::ReplaceString(
		std::string&         str,
		const ::std::string& oldStr,
		const ::std::string& newStr,
		const size_t&        vMaxRecursion
)
{
	if(!str.empty() && oldStr != newStr)
	{
		bool   res          = false;
		size_t pos          = 0;
		bool   found        = false;
		size_t max_recursion= vMaxRecursion;
		do
		{
			pos= str.find(oldStr, pos);
			if(pos != std::string::npos)
			{
				found= res= true;
				str.replace(pos, oldStr.length(), newStr);
				pos+= newStr.length();
			}
			else if(found && max_recursion > 0)
			{ // recursion loop
				found= false;
				pos  = 0;
				--max_recursion;
			}
		} while(pos != std::string::npos);
		return res;
	}
	return false;
}

IGFD_API std::vector<std::string> IGFD::Utils::SplitStringToVector(
		const std::string& vText,
		const char&        vDelimiter,
		const bool&        vPushEmpty
)
{
	std::vector<std::string> arr;
	if(!vText.empty())
	{
		size_t start= 0;
		size_t end  = vText.find(vDelimiter, start);
		while(end != std::string::npos)
		{
			auto token= vText.substr(start, end - start);
			if(!token.empty() || (token.empty() && vPushEmpty))
			{ //-V728
				arr.push_back(token);
			}
			start= end + 1;
			end  = vText.find(vDelimiter, start);
		}
		auto token= vText.substr(start);
		if(!token.empty() || (token.empty() && vPushEmpty))
		{ //-V728
			arr.push_back(token);
		}
	}
	return arr;
}

IGFD_API std::vector<std::string> IGFD::Utils::GetDrivesList()
{
	std::vector<std::string> res;

	#ifdef _IGFD_WIN_
	const DWORD mydrives= 2048;
	char        lpBuffer[2048];
    #define mini(a, b) (((a) < (b)) ? (a) : (b))
	const DWORD countChars= mini(GetLogicalDriveStringsA(mydrives, lpBuffer), 2047);
		#undef mini
	if(countChars > 0U && countChars < 2049U)
	{
		std::string var= std::string(lpBuffer, (size_t)countChars);
		IGFD::Utils::ReplaceString(var, "\\", "");
		res= IGFD::Utils::SplitStringToVector(var, '\0', false);
	}
	#endif // _IGFD_WIN_

	return res;
}

IGFD_API bool IGFD::Utils::IsDirectoryCanBeOpened(const std::string& name)
{
	bool bExists= false;

	if(!name.empty())
	{
		namespace fs= std::filesystem;
	#ifdef _IGFD_WIN_
		std::wstring wname   = IGFD::Utils::UTF8Decode(name.c_str());
		fs::path     pathName= fs::path(wname);
	#else  // _IGFD_WIN_
		fs::path pathName= fs::path(name);
	#endif // _IGFD_WIN_
		try
		{
			// interesting, in the case of a protected dir or for any reason the dir cant be opened
			// this func will work but will say nothing more . not like the dirent version
			bExists= fs::is_directory(pathName);
			// test if can be opened, this function can thrown an exception if there is an issue with this
			// dir here, the dir_iter is need else not exception is thrown..
			const auto dir_iter= std::filesystem::directory_iterator(pathName);
			(void)dir_iter; // for avoid unused warnings
		}
		catch(const std::exception& /*ex*/)
		{
			// fail so this dir cant be opened
			bExists= false;
		}
	}

	return bExists; // this is not a directory!
}

IGFD_API bool IGFD::Utils::IsDirectoryExist(const std::string& name)
{
	bool bExists= false;

	if(!name.empty())
	{
		namespace fs= std::filesystem;
	#ifdef _IGFD_WIN_
		std::wstring wname   = IGFD::Utils::UTF8Decode(name.c_str());
		fs::path     pathName= fs::path(wname);
	#else  // _IGFD_WIN_
		fs::path pathName= fs::path(name);
	#endif // _IGFD_WIN_
		bExists= fs::is_directory(pathName);
	}

	return bExists; // this is not a directory!
}

IGFD_API bool IGFD::Utils::CreateDirectoryIfNotExist(const std::string& name)
{
	bool res= false;

	if(!name.empty())
	{
		if(!IsDirectoryExist(name))
		{
	#ifndef __EMSCRIPTEN__
			namespace fs         = std::filesystem;
			std::wstring wname   = IGFD::Utils::UTF8Decode(name.c_str());
			fs::path     pathName= fs::path(wname);
			res                  = fs::create_directory(pathName);
	#else
			std::string str= std::string("FS.mkdir('") + name + "');";
			emscripten_run_script(str.c_str());
			res= true;

	#endif
			if(!res)
			{
				std::cout << "Error creating directory " << name << std::endl;
			}
		}
	}

	return res;
}

IGFD_API IGFD::Utils::PathStruct IGFD::Utils::ParsePathFileName(const std::string& vPathFileName)
{
	// https://github.com/aiekick/ImGuiFileDialog/issues/54
	namespace fs= std::filesystem;
	PathStruct res;
	if(vPathFileName.empty())
	{
		return res;
	}

	auto fsPath= fs::path(vPathFileName);

	if(fs::is_directory(fsPath))
	{
		res.name= "";
		res.path= fsPath.string();
		res.isOk= true;
	}
	else if(fs::is_regular_file(fsPath))
	{
		res.name= fsPath.filename().string();
		res.path= fsPath.parent_path().string();
		res.isOk= true;
	}

	return res;
}

IGFD_API void IGFD::Utils::AppendToBuffer(char* vBuffer, size_t vBufferLen, const std::string& vStr)
{
	std::string st  = vStr;
	size_t      len = vBufferLen - 1u;
	size_t      slen= strlen(vBuffer);

	if(!st.empty() && st != "\n")
	{
		IGFD::Utils::ReplaceString(st, "\n", "");
		IGFD::Utils::ReplaceString(st, "\r", "");
	}
	vBuffer[slen]  = '\0';
	std::string str= std::string(vBuffer);
	// if (!str.empty()) str += "\n";
	str+= vStr;
	if(len > str.size())
	{
		len= str.size();
	}
	#ifdef _MSC_VER
	strncpy_s(vBuffer, vBufferLen, str.c_str(), len);
	#else  // _MSC_VER
	strncpy(vBuffer, str.c_str(), len);
	#endif // _MSC_VER
	vBuffer[len]= '\0';
}

IGFD_API void IGFD::Utils::ResetBuffer(char* vBuffer) { vBuffer[0]= '\0'; }

IGFD_API void IGFD::Utils::SetBuffer(char* vBuffer, size_t vBufferLen, const std::string& vStr)
{
	ResetBuffer(vBuffer);
	AppendToBuffer(vBuffer, vBufferLen, vStr);
}

IGFD_API std::string IGFD::Utils::LowerCaseString(const std::string& vString)
{
	std::string str= vString;

	// convert to lower case
	for(char& c : str)
	{
		c= (char)std::tolower(c);
	}

	return str;
}

IGFD_API size_t IGFD::Utils::GetCharCountInString(const std::string& vString, const char& vChar)
{
	size_t res= 0U;
	for(const auto& c : vString)
	{
		if(c == vChar)
		{
			++res;
		}
	}
	return res;
}

IGFD_API size_t IGFD::Utils::GetLastCharPosWithMinCharCount(
		const std::string& vString,
		const char&        vChar,
		const size_t&      vMinCharCount
)
{
	if(vMinCharCount)
	{
		size_t last_dot_pos= vString.size() + 1U;
		size_t count_dots  = vMinCharCount;
		while(count_dots > 0U && last_dot_pos > 0U && last_dot_pos != std::string::npos)
		{
			auto new_dot= vString.rfind(vChar, last_dot_pos - 1U);
			if(new_dot != std::string::npos)
			{
				last_dot_pos= new_dot;
				--count_dots;
			}
			else
			{
				break;
			}
		}
		return last_dot_pos;
	}
	return std::string::npos;
}

	#pragma endregion

	#pragma region FileStyle

IGFD_API IGFD::FileStyle::FileStyle(): color(0, 0, 0, 0) { }

IGFD_API IGFD::FileStyle::FileStyle(const FileStyle& vStyle)
{
	color= vStyle.color;
	icon = vStyle.icon;
	font = vStyle.font;
	flags= vStyle.flags;
}

IGFD_API IGFD::FileStyle::FileStyle(const ImVec4& vColor, const std::string& vIcon, ImFont* vFont)
		: color(vColor), icon(vIcon), font(vFont)
{
}

	#pragma endregion

	#pragma region SearchManager

IGFD_API void IGFD::SearchManager::Clear()
{
	puSearchTag.clear();
	IGFD::Utils::ResetBuffer(puSearchBuffer.data());
}

IGFD_API void IGFD::SearchManager::DrawSearchBar(FileDialogInternal& vFileDialogInternal)
{
	// search field
	if(IMGUI_BUTTON(resetButtonString "##BtnImGuiFileDialogSearchField"))
	{
		Clear();
		vFileDialogInternal.puFileManager.ApplyFilteringOnFileList(vFileDialogInternal);
	}
	if(ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(buttonResetSearchString);
	}
	ImGui::SameLine();
	ImGui::Text(searchString);
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
	bool edited= ImGui::InputText(
			"##InputImGuiFileDialogSearchField", puSearchBuffer.data(), MAX_FILE_DIALOG_NAME_BUFFER
	);
	if(ImGui::GetItemID() == ImGui::GetActiveID())
	{
		puSearchInputIsActive= true;
	}
	ImGui::PopItemWidth();
	if(edited)
	{
		puSearchTag= puSearchBuffer.data();
		vFileDialogInternal.puFileManager.ApplyFilteringOnFileList(vFileDialogInternal);
	}
}

	#pragma endregion

	#pragma region FilterInfos

void IGFD::FilterInfos::setCollectionTitle(const std::string& vTitle) { title= vTitle; }

void IGFD::FilterInfos::addFilter(const std::string& vFilter, const bool& vIsRegex)
{
	setCollectionTitle(vFilter);
	addCollectionFilter(vFilter, vIsRegex);
}

void IGFD::FilterInfos::addCollectionFilter(const std::string& vFilter, const bool& vIsRegex)
{
	if(!vIsRegex)
	{
		if(vFilter.find('*') != std::string::npos)
		{
			const auto& regex_string= transformAsteriskBasedFilterToRegex(vFilter);
			addCollectionFilter(regex_string, true);
			return;
		}
		filters.try_add(vFilter);
		filters_optimized.try_add(Utils::LowerCaseString(vFilter));
		auto _count_dots= Utils::GetCharCountInString(vFilter, '.');
		if(_count_dots > count_dots)
		{
			count_dots= _count_dots;
		}
	}
	else
	{
		try
		{
			auto rx= std::regex(vFilter);
			filters.try_add(vFilter);
			filters_regex.emplace_back(rx);
		}
		catch(std::exception&)
		{
			assert(0); // YOUR REGEX FILTER IS INVALID
		}
	}
}

void IGFD::FilterInfos::clear()
{
	title.clear();
	filters.clear();
	filters_optimized.clear();
	filters_regex.clear();
}

bool IGFD::FilterInfos::empty() const { return filters.empty() || filters.begin()->empty(); }

const std::string& IGFD::FilterInfos::getFirstFilter() const
{
	if(!filters.empty())
	{
		return *filters.begin();
	}
	return empty_string;
}

bool IGFD::FilterInfos::exist(const FileInfos& vFileInfos, bool vIsCaseInsensitive) const
{
	for(const auto& filter : filters)
	{
		if(vFileInfos.SearchForExt(filter, vIsCaseInsensitive, count_dots))
		{
			return true;
		}
	}
	return false;
}

bool IGFD::FilterInfos::regexExist(const std::string& vFilter) const
{
	for(auto regex : filters_regex)
	{
		if(std::regex_search(vFilter, regex))
		{
			return true;
		}
	}
	return false;
}

IGFD_API std::string
				 IGFD::FilterInfos::transformAsteriskBasedFilterToRegex(const std::string& vFilter)
{
	std::string res;
	if(!vFilter.empty() && vFilter.find('*') != std::string::npos)
	{
		res= "((";
		for(const auto& c : vFilter)
		{
			if(c == '.')
			{
				res+= "[.]"; // [.] => a dot
			}
			else if(c == '*')
			{
				res+= ".*"; // .* => any char zero or many
			}
			else
			{
				res+= c; // other chars
			}
		}
		res+= "$))"; // $ => end fo the string
	}
	return res;
}

	#pragma endregion

	#pragma region FilterManager

IGFD_API const IGFD::FilterInfos& IGFD::FilterManager::GetSelectedFilter() const
{
	return prSelectedFilter;
}

IGFD_API void IGFD::FilterManager::ParseFilters(const char* vFilters)
{
	prParsedFilters.clear();

	if(vFilters != nullptr)
	{
		puDLGFilters= vFilters; // file mode
	}
	else
	{
		puDLGFilters.clear(); // directory mode
	}

	if(!puDLGFilters.empty())
	{
		/* Rules
		0) a filter must have 2 chars mini and the first must be a .
		1) a regex must be in (( and ))
		2) a , will separate filters except if between a ( and )
		3) name{filter1, filter2} is a spetial form for collection filters
		3.1) the name can be composed of what you want except { and }
		3.2) the filter can be a regex
		4) the filters cannot integrate these chars '(' ')' '{' '}' ' ' except for a regex with respect
		to rule 1) 5) the filters cannot integrate a ','
		*/

		bool current_filter_found= false;
		bool started             = false;
		bool regex_started       = false;
		bool parenthesis_started = false;

		std::string word;
		std::string filter_name;

		char last_split_char= 0;
		for(char c : puDLGFilters)
		{
			if(c == '{')
			{
				if(regex_started)
				{
					word+= c;
				}
				else
				{
					started= true;
					prParsedFilters.emplace_back();
					prParsedFilters.back().setCollectionTitle(filter_name);
					filter_name.clear();
					word.clear();
				}
				last_split_char= c;
			}
			else if(c == '}')
			{
				if(regex_started)
				{
					word+= c;
				}
				else
				{
					if(started)
					{
						if(word.size() > 1U && word[0] == '.')
						{
							if(prParsedFilters.empty())
							{
								prParsedFilters.emplace_back();
							}
							prParsedFilters.back().addCollectionFilter(word, false);
						}
						word.clear();
						filter_name.clear();
						started= false;
					}
				}
				last_split_char= c;
			}
			else if(c == '(')
			{
				word+= c;
				if(last_split_char == '(')
				{
					regex_started= true;
				}
				parenthesis_started= true;
				if(!started)
				{
					filter_name+= c;
				}
				last_split_char= c;
			}
			else if(c == ')')
			{
				word+= c;
				if(last_split_char == ')')
				{
					if(regex_started)
					{
						if(started)
						{
							prParsedFilters.back().addCollectionFilter(word, true);
						}
						else
						{
							prParsedFilters.emplace_back();
							prParsedFilters.back().addFilter(word, true);
						}
						word.clear();
						filter_name.clear();
						regex_started= false;
					}
					else
					{
						if(!started)
						{
							if(!prParsedFilters.empty())
							{
								prParsedFilters.erase(prParsedFilters.begin() + prParsedFilters.size() - 1U);
							}
							else
							{
								prParsedFilters.clear();
							}
						}
						word.clear();
						filter_name.clear();
					}
				}
				parenthesis_started= false;
				if(!started)
				{
					filter_name+= c;
				}
				last_split_char= c;
			}
			else if(c == '.')
			{
				word+= c;
				if(!started)
				{
					filter_name+= c;
				}
				last_split_char= c;
			}
			else if(c == ',')
			{
				if(regex_started)
				{
					regex_started= false;
					word.clear();
					filter_name.clear();
				}
				else
				{
					if(started)
					{
						if(word.size() > 1U && word[0] == '.')
						{
							prParsedFilters.back().addCollectionFilter(word, false);
							word.clear();
							filter_name.clear();
						}
					}
					else
					{
						if(word.size() > 1U && word[0] == '.')
						{
							prParsedFilters.emplace_back();
							prParsedFilters.back().addFilter(word, false);
							word.clear();
							filter_name.clear();
						}
						if(parenthesis_started)
						{
							filter_name+= c;
						}
					}
				}
			}
			else
			{
				if(c != ' ')
				{
					word+= c;
				}
				if(!started)
				{
					filter_name+= c;
				}
			}
		}

		if(started)
		{
			if(!prParsedFilters.empty())
			{
				prParsedFilters.erase(prParsedFilters.begin() + prParsedFilters.size() - 1U);
			}
			else
			{
				prParsedFilters.clear();
			}
		}
		else if(word.size() > 1U && word[0] == '.')
		{
			prParsedFilters.emplace_back();
			prParsedFilters.back().addFilter(word, false);
			word.clear();
		}

		for(const auto& it : prParsedFilters)
		{
			if(it.title == prSelectedFilter.title)
			{
				prSelectedFilter    = it;
				current_filter_found= true;
				break;
			}
		}

		if(!current_filter_found)
		{
			if(!prParsedFilters.empty())
			{
				prSelectedFilter= *prParsedFilters.begin();
			}
		}
	}
}

IGFD_API void IGFD::FilterManager::SetSelectedFilterWithExt(const std::string& vFilter)
{
	if(!prParsedFilters.empty())
	{
		if(!vFilter.empty())
		{
			for(const auto& infos : prParsedFilters)
			{
				for(const auto& filter : infos.filters)
				{
					if(vFilter == filter)
					{
						prSelectedFilter= infos;
					}
				}
			}
		}

		if(prSelectedFilter.empty())
		{
			prSelectedFilter= *prParsedFilters.begin();
		}
	}
}

IGFD_API void IGFD::FilterManager::SetFileStyle(
		const IGFD_FileStyleFlags& vFlags,
		const char*                vCriteria,
		const FileStyle&           vInfos
)
{
	std::string _criteria                 = (vCriteria != nullptr) ? std::string(vCriteria) : "";
	prFilesStyle[vFlags][_criteria]       = std::make_shared<FileStyle>(vInfos);
	prFilesStyle[vFlags][_criteria]->flags= vFlags;
}

// will be called internally
// will not been exposed to IGFD API
IGFD_API bool IGFD::FilterManager::prFillFileStyle(std::shared_ptr<FileInfos> vFileInfos) const
{
	// todo : better system to found regarding what style to priorize regarding other
	// maybe with a lambda fucntion for let the user use his style
	// according to his use case
	if(vFileInfos.use_count() && !prFilesStyle.empty())
	{
		for(const auto& _flag : prFilesStyle)
		{
			for(const auto& _file : _flag.second)
			{
				if((_flag.first & IGFD_FileStyleByTypeDir && _flag.first & IGFD_FileStyleByTypeLink &&
						vFileInfos->fileType.isDir() && vFileInfos->fileType.isSymLink()) ||
					 (_flag.first & IGFD_FileStyleByTypeFile && _flag.first & IGFD_FileStyleByTypeLink &&
						vFileInfos->fileType.isFile() && vFileInfos->fileType.isSymLink()) ||
					 (_flag.first & IGFD_FileStyleByTypeLink && vFileInfos->fileType.isSymLink()) ||
					 (_flag.first & IGFD_FileStyleByTypeDir && vFileInfos->fileType.isDir()) ||
					 (_flag.first & IGFD_FileStyleByTypeFile && vFileInfos->fileType.isFile()))
				{
					if(_file.first.empty())
					{ // for all links
						vFileInfos->fileStyle= _file.second;
					}
					else if(_file.first.find("((") != std::string::npos &&
									std::regex_search(vFileInfos->fileNameExt, std::regex(_file.first)))
					{ // for links who are equal to style criteria
						vFileInfos->fileStyle= _file.second;
					}
					else if(_file.first == vFileInfos->fileNameExt)
					{ // for links who are equal to style criteria
						vFileInfos->fileStyle= _file.second;
					}
				}

				if(_flag.first & IGFD_FileStyleByExtention)
				{
					if(_file.first.find("((") != std::string::npos &&
						 std::regex_search(vFileInfos->fileExtLevels[0], std::regex(_file.first)))
					{
						vFileInfos->fileStyle= _file.second;
					}
					else if(vFileInfos->SearchForExt(_file.first, false))
					{
						vFileInfos->fileStyle= _file.second;
					}
				}

				if(_flag.first & IGFD_FileStyleByFullName)
				{
					if(_file.first.find("((") != std::string::npos &&
						 std::regex_search(vFileInfos->fileNameExt, std::regex(_file.first)))
					{
						vFileInfos->fileStyle= _file.second;
					}
					else if(_file.first == vFileInfos->fileNameExt)
					{
						vFileInfos->fileStyle= _file.second;
					}
				}

				if(_flag.first & IGFD_FileStyleByContainedInFullName)
				{
					if(_file.first.find("((") != std::string::npos &&
						 std::regex_search(vFileInfos->fileNameExt, std::regex(_file.first)))
					{
						vFileInfos->fileStyle= _file.second;
					}
					else if(vFileInfos->fileNameExt.find(_file.first) != std::string::npos)
					{
						vFileInfos->fileStyle= _file.second;
					}
				}

				for(auto& functor : prFilesStyleFunctors)
				{
					if(functor)
					{
						FileStyle result;
						if(functor(*(vFileInfos.get()), result))
						{
							vFileInfos->fileStyle= std::make_shared<FileStyle>(std::move(result));
						}
					}
				}

				if(vFileInfos->fileStyle.use_count())
				{
					return true;
				}
			}
		}
	}

	return false;
}

IGFD_API void IGFD::FilterManager::SetFileStyle(
		const IGFD_FileStyleFlags& vFlags,
		const char*                vCriteria,
		const ImVec4&              vColor,
		const std::string&         vIcon,
		ImFont*                    vFont
)
{
	std::string _criteria;
	if(vCriteria != nullptr)
	{
		_criteria= std::string(vCriteria);
	}
	prFilesStyle[vFlags][_criteria]       = std::make_shared<FileStyle>(vColor, vIcon, vFont);
	prFilesStyle[vFlags][_criteria]->flags= vFlags;
}

IGFD_API void IGFD::FilterManager::SetFileStyle(FileStyle::FileStyleFunctor vFunctor)
{
	if(vFunctor)
	{
		prFilesStyleFunctors.push_back(vFunctor);
	}
}

// todo : refactor this fucking function
IGFD_API bool IGFD::FilterManager::GetFileStyle(
		const IGFD_FileStyleFlags& vFlags,
		const std::string&         vCriteria,
		ImVec4*                    vOutColor,
		std::string* const         vOutIcon,
		ImFont**                   vOutFont
)
{
	if(vOutColor != nullptr)
	{
		if(!prFilesStyle.empty())
		{
			if(prFilesStyle.find(vFlags) != prFilesStyle.end())
			{ // found
				if(vFlags & IGFD_FileStyleByContainedInFullName)
				{
					// search for vCriteria who are containing the criteria
					for(const auto& _file : prFilesStyle.at(vFlags))
					{
						if(vCriteria.find(_file.first) != std::string::npos)
						{
							if(_file.second.use_count())
							{
								*vOutColor= _file.second->color;
								if(vOutIcon)
								{
									*vOutIcon= _file.second->icon;
								}
								if(vOutFont)
								{
									*vOutFont= _file.second->font;
								}
								return true;
							}
						}
					}
				}
				else
				{
					if(prFilesStyle.at(vFlags).find(vCriteria) != prFilesStyle.at(vFlags).end())
					{ // found
						*vOutColor= prFilesStyle[vFlags][vCriteria]->color;
						if(vOutIcon != nullptr)
						{
							*vOutIcon= prFilesStyle[vFlags][vCriteria]->icon;
						}
						if(vOutFont != nullptr)
						{
							*vOutFont= prFilesStyle[vFlags][vCriteria]->font;
						}
						return true;
					}
				}
			}
			else
			{
				// search for flag composition
				for(const auto& _flag : prFilesStyle)
				{
					if(_flag.first & vFlags)
					{
						if(_flag.first & IGFD_FileStyleByContainedInFullName)
						{
							// search for vCriteria who are containing the criteria
							for(const auto& _file : prFilesStyle.at(_flag.first))
							{
								if(vCriteria.find(_file.first) != std::string::npos)
								{
									if(_file.second.use_count())
									{
										*vOutColor= _file.second->color;
										if(vOutIcon)
										{
											*vOutIcon= _file.second->icon;
										}
										if(vOutFont)
										{
											*vOutFont= _file.second->font;
										}
										return true;
									}
								}
							}
						}
						else
						{
							if(prFilesStyle.at(_flag.first).find(vCriteria) != prFilesStyle.at(_flag.first).end())
							{ // found
								*vOutColor= prFilesStyle[_flag.first][vCriteria]->color;
								if(vOutIcon)
								{
									*vOutIcon= prFilesStyle[_flag.first][vCriteria]->icon;
								}
								if(vOutFont)
								{
									*vOutFont= prFilesStyle[_flag.first][vCriteria]->font;
								}
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

IGFD_API void IGFD::FilterManager::ClearFilesStyle() { prFilesStyle.clear(); }

IGFD_API bool
IGFD::FilterManager::IsCoveredByFilters(const FileInfos& vFileInfos, bool vIsCaseInsensitive) const
{
	if(!puDLGFilters.empty() && !prSelectedFilter.empty())
	{
		return (
				prSelectedFilter.exist(vFileInfos, vIsCaseInsensitive) ||
				prSelectedFilter.regexExist(vFileInfos.fileNameExt)
		);
	}

	return false;
}

IGFD_API float IGFD::FilterManager::GetFilterComboBoxWidth() const
{
	#if FILTER_COMBO_AUTO_SIZE
	const auto& combo_width= ImGui::CalcTextSize(prSelectedFilter.title.c_str()).x +
													 ImGui::GetFrameHeight() + ImGui::GetStyle().ItemInnerSpacing.x;
	return ImMax(combo_width, FILTER_COMBO_MIN_WIDTH);
	#else
	return FILTER_COMBO_MIN_WIDTH;
	#endif
}

IGFD_API bool IGFD::FilterManager::DrawFilterComboBox(FileDialogInternal& vFileDialogInternal)
{
	if(!puDLGFilters.empty())
	{
		ImGui::SameLine();
		bool needToApllyNewFilter= false;
		ImGui::PushItemWidth(GetFilterComboBoxWidth());
		if(IMGUI_BEGIN_COMBO("##Filters", prSelectedFilter.title.c_str(), ImGuiComboFlags_None))
		{
			intptr_t i= 0;
			for(const auto& filter : prParsedFilters)
			{
				const bool item_selected= (filter.title == prSelectedFilter.title);
				ImGui::PushID((void*)(intptr_t)i++);
				if(ImGui::Selectable(filter.title.c_str(), item_selected))
				{
					prSelectedFilter    = filter;
					needToApllyNewFilter= true;
				}
				ImGui::PopID();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		if(needToApllyNewFilter)
		{
			vFileDialogInternal.puFileManager.OpenCurrentPath(vFileDialogInternal);
		}
		return needToApllyNewFilter;
	}
	return false;
}

IGFD_API std::string IGFD::FilterManager::ReplaceExtentionWithCurrentFilterIfNeeded(
		const std::string& vFileName,
		IGFD_ResultMode    vFlag
) const
{
	std::string result= vFileName;
	if(!result.empty())
	{
		const auto& current_filter= prSelectedFilter.getFirstFilter();
		if(!current_filter.empty())
		{
			Utils::ReplaceString(result, "..", ".");

			// is a regex => no change
			if(current_filter.find("((") != std::string::npos)
			{
				return result;
			}

			// contain .* => no change
			if(current_filter.find(".*") != std::string::npos)
			{
				return result;
			}

			switch(vFlag)
			{
				case IGFD_ResultMode_KeepInputFile:
					{
						return vFileName;
					}
				case IGFD_ResultMode_OverwriteFileExt:
					{
						const auto& count_dots= Utils::GetCharCountInString(vFileName, '.');
						const auto& min_dots  = ImMin<size_t>(count_dots, prSelectedFilter.count_dots);
						const auto& lp        = Utils::GetLastCharPosWithMinCharCount(vFileName, '.', min_dots);
						if(lp != std::string::npos)
						{ // there is a user extention
							const auto& file_name_without_user_ext= vFileName.substr(0, lp);
							result                                = file_name_without_user_ext + current_filter;
						}
						else
						{ // add extention
							result= vFileName + current_filter;
						}
						break;
					}
				case IGFD_ResultMode_AddIfNoFileExt:
					{
						const auto& count_dots= Utils::GetCharCountInString(vFileName, '.');
						const auto& min_dots  = ImMin<size_t>(count_dots, prSelectedFilter.count_dots);
						const auto& lp        = Utils::GetLastCharPosWithMinCharCount(vFileName, '.', min_dots);
						if(lp == std::string::npos || // there is no user extention
							 lp == (vFileName.size() - 1U))
						{ // or this pos is also the last char => considered like no user extention
							const auto& file_name_without_user_ext= vFileName.substr(0, lp);
							result                                = file_name_without_user_ext + current_filter;
						}
						break;
					}
			}

			Utils::ReplaceString(result, "..", ".");
		}
	}
	return result;
}

IGFD_API void IGFD::FilterManager::SetDefaultFilterIfNotDefined()
{
	if(prSelectedFilter.empty() && // no filter selected
		 !prParsedFilters.empty())
	{                                             // filter exist
		prSelectedFilter= *prParsedFilters.begin(); // we take the first filter
	}
}

	#pragma endregion

	#pragma region FileType

IGFD::FileType::FileType()= default;

IGFD::FileType::FileType(const ContentType& vContentType, const bool& vIsSymlink)
		: m_Content(vContentType), m_Symlink(vIsSymlink)
{
}

void IGFD::FileType::SetContent(const ContentType& vContentType) { m_Content= vContentType; }

void IGFD::FileType::SetSymLink(const bool& vIsSymlink) { m_Symlink= vIsSymlink; }

bool IGFD::FileType::isValid() const { return m_Content != ContentType::Invalid; }

bool IGFD::FileType::isDir() const { return m_Content == ContentType::Directory; }

bool IGFD::FileType::isFile() const { return m_Content == ContentType::File; }

bool IGFD::FileType::isLinkToUnknown() const { return m_Content == ContentType::LinkToUnknown; }

bool IGFD::FileType::isSymLink() const { return m_Symlink; }

// Comparisons only care about the content type, ignoring whether it's a symlink or not.
bool IGFD::FileType::operator==(const FileType& rhs) const { return m_Content == rhs.m_Content; }

bool IGFD::FileType::operator!=(const FileType& rhs) const { return m_Content != rhs.m_Content; }

bool IGFD::FileType::operator<(const FileType& rhs) const { return m_Content < rhs.m_Content; }

bool IGFD::FileType::operator>(const FileType& rhs) const { return m_Content > rhs.m_Content; }

	#pragma endregion

	#pragma region FileInfos

IGFD_API bool IGFD::FileInfos::SearchForTag(const std::string& vTag) const
{
	if(!vTag.empty())
	{
		if(fileNameExt_optimized == "..")
		{
			return true;
		}

		return fileNameExt_optimized.find(vTag) !=
							 std::string::npos ||                     // first try without case and accents
					 fileNameExt.find(vTag) != std::string::npos; // second if searched with case and accents
	}

	// if tag is empty => its a special case but all is found
	return true;
}

IGFD_API bool IGFD::FileInfos::SearchForExt(
		const std::string& vExt,
		const bool&        vIsCaseInsensitive,
		const size_t&      vMaxLevel
) const
{
	if(!vExt.empty())
	{
		const auto& ext_levels= vIsCaseInsensitive ? fileExtLevels_optimized : fileExtLevels;
		if(vMaxLevel >= 1 && countExtDot >= vMaxLevel)
		{
			for(const auto& ext : ext_levels)
			{
				if(!ext.empty() && ext == vExt)
				{
					return true;
				}
			}
		}
		else
		{
			return (fileExtLevels[0] == vExt);
		}
	}
	return false;
}

IGFD_API bool IGFD::FileInfos::SearchForExts(
		const std::string& vComaSepExts,
		const bool&        vIsCaseInsensitive,
		const size_t&      vMaxLevel
) const
{
	if(!vComaSepExts.empty())
	{
		const auto& arr= Utils::SplitStringToVector(vComaSepExts, ',', false);
		for(const auto& a : arr)
		{
			if(SearchForExt(a, vIsCaseInsensitive, vMaxLevel))
			{
				return true;
			}
		}
	}
	return false;
}

bool IGFD::FileInfos::FinalizeFileTypeParsing(const size_t& vMaxDotToExtract)
{
	if(fileType.isFile() || fileType.isLinkToUnknown())
	{ // link can have the same extention of a file
		countExtDot= Utils::GetCharCountInString(fileNameExt, '.');
		size_t lpt = 0U;
		if(countExtDot > 1U)
		{ // multi layer ext
			size_t max_dot_to_extract= vMaxDotToExtract;
			if(max_dot_to_extract > countExtDot)
			{
				max_dot_to_extract= countExtDot;
			}
			lpt= Utils::GetLastCharPosWithMinCharCount(fileNameExt, '.', max_dot_to_extract);
		}
		else
		{
			lpt= fileNameExt.find_first_of('.');
		}
		if(lpt != std::string::npos)
		{
			size_t lvl                  = 0U;
			fileExtLevels[lvl]          = fileNameExt.substr(lpt);
			fileExtLevels_optimized[lvl]= Utils::LowerCaseString(fileExtLevels[lvl]);
			if(countExtDot > 1U)
			{ // multi layer ext
				auto count= countExtDot;
				while(count > 0 && lpt != std::string::npos && lvl < fileExtLevels.size())
				{
					++lpt, ++lvl;
					if(fileNameExt.size() > lpt)
					{
						lpt= fileNameExt.find_first_of('.', lpt);
						if(lpt != std::string::npos)
						{
							fileExtLevels[lvl]          = fileNameExt.substr(lpt);
							fileExtLevels_optimized[lvl]= Utils::LowerCaseString(fileExtLevels[lvl]);
						}
					}
				}
			}
		}
		return true;
	}
	return false;
}

	#pragma endregion

	#pragma region FileManager

IGFD_API IGFD::FileManager::FileManager() { puFsRoot= std::string(1u, PATH_SEP); }

IGFD_API void IGFD::FileManager::OpenCurrentPath(const FileDialogInternal& vFileDialogInternal)
{
	puShowDrives= false;
	ClearComposer();
	ClearFileLists();
	if(puDLGDirectoryMode)
	{ // directory mode
		SetDefaultFileName(".");
	}
	else
	{
		SetDefaultFileName(puDLGDefaultFileName);
	}
	ScanDir(vFileDialogInternal, GetCurrentPath());
}

IGFD_API void IGFD::FileManager::SortFields(const FileDialogInternal& vFileDialogInternal)
{
	SortFields(vFileDialogInternal, prFileList, prFilteredFileList);
}

IGFD_API void IGFD::FileManager::SortFields(
		const FileDialogInternal&                vFileDialogInternal,
		std::vector<std::shared_ptr<FileInfos>>& vFileInfosList,
		std::vector<std::shared_ptr<FileInfos>>& vFileInfosFilteredList
)
{
	if(puSortingField != SortingFieldEnum::FIELD_NONE)
	{
		puHeaderFileName= tableHeaderFileNameString;
		puHeaderFileType= tableHeaderFileTypeString;
		puHeaderFileSize= tableHeaderFileSizeString;
		puHeaderFileDate= tableHeaderFileDateString;
	#ifdef USE_THUMBNAILS
		puHeaderFileThumbnails= tableHeaderFileThumbnailsString;
	#endif // #ifdef USE_THUMBNAILS
	}
	if(puSortingField == SortingFieldEnum::FIELD_FILENAME)
	{
		if(puSortingDirection[0])
		{
	#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileName= tableHeaderAscendingIcon + puHeaderFileName;
	#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						// tofix : this code fail in c:\\Users with the link "All users". got a invalid
						// comparator
						/*
						// use code from
						https://github.com/jackm97/ImGuiFileDialog/commit/bf40515f5a1de3043e60562dc1a494ee7ecd3571
						// strict ordering for file/directory types beginning in '.'
						// common on _IGFD_WIN_ platforms
						if (a->fileNameExt[0] == '.' && b->fileNameExt[0] != '.')
								return false;
						if (a->fileNameExt[0] != '.' && b->fileNameExt[0] == '.')
								return true;
						if (a->fileNameExt[0] == '.' && b->fileNameExt[0] == '.')
						{
								return (stricmp(a->fileNameExt.c_str(), b->fileNameExt.c_str()) < 0); // sort
						in insensitive case
						}
						*/
						if(a->fileType != b->fileType)
						{
							return (a->fileType < b->fileType); // directories first
						}
						return (
								stricmp(a->fileNameExt.c_str(), b->fileNameExt.c_str()) < 0
						); // sort in insensitive case
					}
			);
		}
		else
		{
	#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileName= tableHeaderDescendingIcon + puHeaderFileName;
	#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						// tofix : this code fail in c:\\Users with the link "All users". got a invalid
						// comparator
						/*
						// use code from
						https://github.com/jackm97/ImGuiFileDialog/commit/bf40515f5a1de3043e60562dc1a494ee7ecd3571
						// strict ordering for file/directory types beginning in '.'
						// common on _IGFD_WIN_ platforms
						if (a->fileNameExt[0] == '.' && b->fileNameExt[0] != '.')
								return false;
						if (a->fileNameExt[0] != '.' && b->fileNameExt[0] == '.')
								return true;
						if (a->fileNameExt[0] == '.' && b->fileNameExt[0] == '.')
						{
								return (stricmp(a->fileNameExt.c_str(), b->fileNameExt.c_str()) > 0); // sort
						in insensitive case
						}
						*/
						if(a->fileType != b->fileType)
						{
							return (a->fileType > b->fileType); // directories last
						}
						return (
								stricmp(a->fileNameExt.c_str(), b->fileNameExt.c_str()) > 0
						); // sort in insensitive case
					}
			);
		}
	}
	else if(puSortingField == SortingFieldEnum::FIELD_TYPE)
	{
		if(puSortingDirection[1])
		{
	#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileType= tableHeaderAscendingIcon + puHeaderFileType;
	#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						if(a->fileType != b->fileType)
						{
							return (a->fileType < b->fileType); // directory in first
						}
						return (a->fileExtLevels[0] < b->fileExtLevels[0]); // else
					}
			);
		}
		else
		{
	#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileType= tableHeaderDescendingIcon + puHeaderFileType;
	#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						if(a->fileType != b->fileType)
						{
							return (a->fileType > b->fileType); // directory in last
						}
						return (a->fileExtLevels[0] > b->fileExtLevels[0]); // else
					}
			);
		}
	}
	else if(puSortingField == SortingFieldEnum::FIELD_SIZE)
	{
		if(puSortingDirection[2])
		{
	#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileSize= tableHeaderAscendingIcon + puHeaderFileSize;
	#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						if(a->fileType != b->fileType)
						{
							return (a->fileType < b->fileType); // directory in first
						}
						return (a->fileSize < b->fileSize); // else
					}
			);
		}
		else
		{
	#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileSize= tableHeaderDescendingIcon + puHeaderFileSize;
	#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						if(a->fileType != b->fileType)
						{
							return (a->fileType > b->fileType); // directory in last
						}
						return (a->fileSize > b->fileSize); // else
					}
			);
		}
	}
	else if(puSortingField == SortingFieldEnum::FIELD_DATE)
	{
		if(puSortingDirection[3])
		{
	#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileDate= tableHeaderAscendingIcon + puHeaderFileDate;
	#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						if(a->fileType != b->fileType)
						{
							return (a->fileType < b->fileType); // directory in first
						}
						return (a->fileModifDate < b->fileModifDate); // else
					}
			);
		}
		else
		{
	#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileDate= tableHeaderDescendingIcon + puHeaderFileDate;
	#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						if(a->fileType != b->fileType)
						{
							return (a->fileType > b->fileType); // directory in last
						}
						return (a->fileModifDate > b->fileModifDate); // else
					}
			);
		}
	}
	#ifdef USE_THUMBNAILS
	else if(puSortingField == SortingFieldEnum::FIELD_THUMBNAILS)
	{
		// we will compare thumbnails by :
		// 1) width
		// 2) height

		if(puSortingDirection[4])
		{
		#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileThumbnails= tableHeaderAscendingIcon + puHeaderFileThumbnails;
		#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						if(a->fileType != b->fileType)
						{
							return (a->fileType.isDir()); // directory in first
						}
						if(a->thumbnailInfo.textureWidth == b->thumbnailInfo.textureWidth)
						{
							return (a->thumbnailInfo.textureHeight < b->thumbnailInfo.textureHeight);
						}
						return (a->thumbnailInfo.textureWidth < b->thumbnailInfo.textureWidth);
					}
			);
		}

		else
		{
		#ifdef USE_CUSTOM_SORTING_ICON
			puHeaderFileThumbnails= tableHeaderDescendingIcon + puHeaderFileThumbnails;
		#endif // USE_CUSTOM_SORTING_ICON
			std::sort(
					vFileInfosList.begin(), vFileInfosList.end(),
					[](const std::shared_ptr<FileInfos>& a, const std::shared_ptr<FileInfos>& b) -> bool
					{
						if(!a.use_count() || !b.use_count())
						{
							return false;
						}
						if(a->fileType != b->fileType)
						{
							return (!a->fileType.isDir()); // directory in last
						}
						if(a->thumbnailInfo.textureWidth == b->thumbnailInfo.textureWidth)
						{
							return (a->thumbnailInfo.textureHeight > b->thumbnailInfo.textureHeight);
						}
						return (a->thumbnailInfo.textureWidth > b->thumbnailInfo.textureWidth);
					}
			);
		}
	}
	#endif // USE_THUMBNAILS

	ApplyFilteringOnFileList(vFileDialogInternal, vFileInfosList, vFileInfosFilteredList);
}

IGFD_API void IGFD::FileManager::ClearFileLists()
{
	prFilteredFileList.clear();
	prFileList.clear();
}

IGFD_API void IGFD::FileManager::ClearPathLists()
{
	prFilteredPathList.clear();
	prPathList.clear();
}

IGFD_API void IGFD::FileManager::AddFile(
		const FileDialogInternal& vFileDialogInternal,
		const std::string&        vPath,
		const std::string&        vFileName,
		const FileType&           vFileType
)
{
	auto infos= std::make_shared<FileInfos>();

	infos->filePath             = vPath;
	infos->fileNameExt          = vFileName;
	infos->fileNameExt_optimized= Utils::LowerCaseString(infos->fileNameExt);
	infos->fileType             = vFileType;

	if(infos->fileNameExt.empty() ||
		 (infos->fileNameExt == "." && !vFileDialogInternal.puFilterManager.puDLGFilters.empty()))
	{ // filename empty or filename is the current dir '.' //-V807
		return;
	}

	if(infos->fileNameExt != ".." &&
		 (vFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DontShowHiddenFiles) &&
		 infos->fileNameExt[0] == '.')
	{ // dont show hidden files
		if(!vFileDialogInternal.puFilterManager.puDLGFilters.empty() ||
			 (vFileDialogInternal.puFilterManager.puDLGFilters.empty() && infos->fileNameExt != "."))
		{ // except "." if in directory mode //-V728
			return;
		}
	}

	if(infos->FinalizeFileTypeParsing(
				 vFileDialogInternal.puFilterManager.GetSelectedFilter().count_dots
		 ))
	{
		if(!vFileDialogInternal.puFilterManager.IsCoveredByFilters(
					 *infos.get(),
					 (vFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_CaseInsensitiveExtention) != 0
			 ))
		{
			return;
		}
	}

	vFileDialogInternal.puFilterManager.prFillFileStyle(infos);

	prCompleteFileInfos(infos);
	prFileList.push_back(infos);
}

IGFD_API void IGFD::FileManager::AddPath(
		const FileDialogInternal& vFileDialogInternal,
		const std::string&        vPath,
		const std::string&        vFileName,
		const FileType&           vFileType
)
{
	if(!vFileType.isDir())
	{
		return;
	}

	auto infos= std::make_shared<FileInfos>();

	infos->filePath             = vPath;
	infos->fileNameExt          = vFileName;
	infos->fileNameExt_optimized= Utils::LowerCaseString(infos->fileNameExt);
	infos->fileType             = vFileType;

	if(infos->fileNameExt.empty() ||
		 (infos->fileNameExt == "." && !vFileDialogInternal.puFilterManager.puDLGFilters.empty()))
	{ // filename empty or filename is the current dir '.' //-V807
		return;
	}

	if(infos->fileNameExt != ".." &&
		 (vFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DontShowHiddenFiles) &&
		 infos->fileNameExt[0] == '.')
	{ // dont show hidden files
		if(!vFileDialogInternal.puFilterManager.puDLGFilters.empty() ||
			 (vFileDialogInternal.puFilterManager.puDLGFilters.empty() && infos->fileNameExt != "."))
		{ // except "." if in directory mode //-V728
			return;
		}
	}

	vFileDialogInternal.puFilterManager.prFillFileStyle(infos);

	prCompleteFileInfos(infos);
	prPathList.push_back(infos);
}

IGFD_API void
IGFD::FileManager::ScanDir(const FileDialogInternal& vFileDialogInternal, const std::string& vPath)
{
	std::string path= vPath;

	if(prCurrentPathDecomposition.empty())
	{
		SetCurrentDir(path);
	}

	if(!prCurrentPathDecomposition.empty())
	{
	#ifdef _IGFD_WIN_
		if(path == puFsRoot)
		{
			path+= std::string(1u, PATH_SEP);
		}
	#endif // _IGFD_WIN_

		ClearFileLists();
		try
		{
			const std::filesystem::path fspath(path);
			const auto                  dir_iter= std::filesystem::directory_iterator(fspath);
			FileType                    fstype  = FileType(
          FileType::ContentType::Directory,
          std::filesystem::is_symlink(std::filesystem::status(fspath))
      );
			AddFile(vFileDialogInternal, path, "..", fstype);
			for(const auto& file : dir_iter)
			{
				FileType fileType;
				if(file.is_symlink())
				{
					fileType.SetSymLink(file.is_symlink());
					fileType.SetContent(FileType::ContentType::LinkToUnknown);
				}

				if(file.is_directory())
				{
					fileType.SetContent(FileType::ContentType::Directory);
				} // directory or symlink to directory
				else if(file.is_regular_file())
				{
					fileType.SetContent(FileType::ContentType::File);
				}

				if(fileType.isValid())
				{
					auto fileNameExt= file.path().filename().string();
					AddFile(vFileDialogInternal, path, fileNameExt, fileType);
				}
			}
		}
		catch(const std::exception& ex)
		{
			printf("%s", ex.what());
		}
		SortFields(vFileDialogInternal, prFileList, prFilteredFileList);
	}
}

IGFD_API void IGFD::FileManager::ScanDirForPathSelection(
		const FileDialogInternal& vFileDialogInternal,
		const std::string&        vPath
)
{
	std::string path= vPath;

	if(!path.empty())
	{
	#ifdef _IGFD_WIN_
		if(path == puFsRoot)
		{
			path+= std::string(1u, PATH_SEP);
		}
	#endif // _IGFD_WIN_

		ClearPathLists();

		const std::filesystem::path fspath(path);
		const auto                  dir_iter= std::filesystem::directory_iterator(fspath);
		FileType                    fstype  = FileType(
        FileType::ContentType::Directory,
        std::filesystem::is_symlink(std::filesystem::status(fspath))
    );
		AddPath(vFileDialogInternal, path, "..", fstype);
		for(const auto& file : dir_iter)
		{
			FileType fileType;
			if(file.is_symlink())
			{
				fileType.SetSymLink(file.is_symlink());
				fileType.SetContent(FileType::ContentType::LinkToUnknown);
			}
			if(file.is_directory())
			{
				fileType.SetContent(FileType::ContentType::Directory);
				auto fileNameExt= file.path().filename().string();
				AddPath(vFileDialogInternal, path, fileNameExt, fileType);
			}
		}

		SortFields(vFileDialogInternal, prPathList, prFilteredPathList);
	}
}

IGFD_API void IGFD::FileManager::OpenPathPopup(
		const FileDialogInternal&          vFileDialogInternal,
		std::vector<std::string>::iterator vPathIter
)
{
	const auto path= ComposeNewPath(vPathIter);
	ScanDirForPathSelection(vFileDialogInternal, path);
	prPopupComposedPath= vPathIter;
	ImGui::OpenPopup("IGFD_Path_Popup");
}

IGFD_API bool IGFD::FileManager::GetDrives()
{
	auto drives= IGFD::Utils::GetDrivesList();
	if(!drives.empty())
	{
		prCurrentPath.clear();
		prCurrentPathDecomposition.clear();
		ClearFileLists();
		for(auto& drive : drives)
		{
			auto info                  = std::make_shared<FileInfos>();
			info->fileNameExt          = drive;
			info->fileNameExt_optimized= Utils::LowerCaseString(drive);
			info->fileType.SetContent(FileType::ContentType::Directory);

			if(!info->fileNameExt.empty())
			{
				prFileList.push_back(info);
			}
		}
		puShowDrives= true;
		return true;
	}
	return false;
}

IGFD_API bool IGFD::FileManager::IsComposerEmpty() { return prCurrentPathDecomposition.empty(); }

IGFD_API size_t IGFD::FileManager::GetComposerSize() { return prCurrentPathDecomposition.size(); }

IGFD_API bool IGFD::FileManager::IsFileListEmpty() { return prFileList.empty(); }

IGFD_API bool IGFD::FileManager::IsPathListEmpty() { return prPathList.empty(); }

IGFD_API size_t IGFD::FileManager::GetFullFileListSize() { return prFileList.size(); }

std::shared_ptr<IGFD::FileInfos> IGFD::FileManager::GetFullFileAt(size_t vIdx)
{
	if(vIdx < prFileList.size())
	{
		return prFileList[vIdx];
	}
	return nullptr;
}

IGFD_API bool IGFD::FileManager::IsFilteredListEmpty() { return prFilteredFileList.empty(); }

IGFD_API bool IGFD::FileManager::IsPathFilteredListEmpty() { return prFilteredPathList.empty(); }

IGFD_API size_t IGFD::FileManager::GetFilteredListSize() { return prFilteredFileList.size(); }

IGFD_API size_t IGFD::FileManager::GetPathFilteredListSize() { return prFilteredPathList.size(); }

IGFD_API std::shared_ptr<IGFD::FileInfos> IGFD::FileManager::GetFilteredFileAt(size_t vIdx)
{
	if(vIdx < prFilteredFileList.size())
	{
		return prFilteredFileList[vIdx];
	}
	return nullptr;
}

IGFD_API std::shared_ptr<IGFD::FileInfos> IGFD::FileManager::GetFilteredPathAt(size_t vIdx)
{
	if(vIdx < prFilteredPathList.size())
	{
		return prFilteredPathList[vIdx];
	}
	return nullptr;
}

IGFD_API std::vector<std::string>::iterator IGFD::FileManager::GetCurrentPopupComposedPath()
{
	return prPopupComposedPath;
}

IGFD_API bool IGFD::FileManager::IsFileNameSelected(const std::string& vFileName)
{
	return prSelectedFileNames.find(vFileName) != prSelectedFileNames.end();
}

IGFD_API std::string IGFD::FileManager::GetBack() { return prCurrentPathDecomposition.back(); }

IGFD_API void IGFD::FileManager::ClearComposer() { prCurrentPathDecomposition.clear(); }

IGFD_API void IGFD::FileManager::ClearAll()
{
	ClearComposer();
	ClearFileLists();
	ClearPathLists();
}

IGFD_API void
IGFD::FileManager::ApplyFilteringOnFileList(const FileDialogInternal& vFileDialogInternal)
{
	ApplyFilteringOnFileList(vFileDialogInternal, prFileList, prFilteredFileList);
}

IGFD_API void IGFD::FileManager::ApplyFilteringOnFileList(
		const FileDialogInternal&                vFileDialogInternal,
		std::vector<std::shared_ptr<FileInfos>>& vFileInfosList,
		std::vector<std::shared_ptr<FileInfos>>& vFileInfosFilteredList
)
{
	vFileInfosFilteredList.clear();
	for(const auto& file : vFileInfosList)
	{
		if(!file.use_count())
		{
			continue;
		}
		bool show= true;
		if(!file->SearchForTag(vFileDialogInternal.puSearchManager.puSearchTag)) // if search tag
		{
			show= false;
		}
		if(puDLGDirectoryMode && !file->fileType.isDir())
		{
			show= false;
		}
		if(show)
		{
			vFileInfosFilteredList.push_back(file);
		}
	}
}

IGFD_API std::string IGFD::FileManager::prRoundNumber(double vvalue, int n)
{
	std::stringstream tmp;
	tmp << std::setprecision(n) << std::fixed << vvalue;
	return tmp.str();
}

IGFD_API std::string IGFD::FileManager::prFormatFileSize(size_t vByteSize)
{
	if(vByteSize != 0)
	{
		static double lo= 1024.0;
		static double ko= 1024.0 * 1024.0;
		static double mo= 1024.0 * 1024.0 * 1024.0;

		auto v= (double)vByteSize;

		if(v < lo)
		{
			return prRoundNumber(v, 0) + " " + fileSizeBytes; // octet
		}
		else if(v < ko)
		{
			return prRoundNumber(v / lo, 2) + " " + fileSizeKiloBytes; // ko
		}
		else if(v < mo)
		{
			return prRoundNumber(v / ko, 2) + " " + fileSizeMegaBytes; // Mo
		}
		else
		{
			return prRoundNumber(v / mo, 2) + " " + fileSizeGigaBytes; // Go
		}
	}

	return "";
}

IGFD_API void IGFD::FileManager::prCompleteFileInfos(const std::shared_ptr<FileInfos>& vInfos)
{
	if(!vInfos.use_count())
	{
		return;
	}

	if(vInfos->fileNameExt != "." && vInfos->fileNameExt != "..")
	{
		// _stat struct :
		// dev_t     st_dev;     /* ID of device containing file */
		// ino_t     st_ino;     /* inode number */
		// mode_t    st_mode;    /* protection */
		// nlink_t   st_nlink;   /* number of hard links */
		// uid_t     st_uid;     /* user ID of owner */
		// gid_t     st_gid;     /* group ID of owner */
		// dev_t     st_rdev;    /* device ID (if special file) */
		// off_t     st_size;    /* total size, in bytes */
		// blksize_t st_blksize; /* blocksize for file system I/O */
		// blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
		// time_t    st_atime;   /* time of last access - not sure out of ntfs */
		// time_t    st_mtime;   /* time of last modification - not sure out of ntfs */
		// time_t    st_ctime;   /* time of last status change - not sure out of ntfs */

		std::string fpn;

		// FIXME: so the condition is always true?
		if(vInfos->fileType.isFile() || vInfos->fileType.isLinkToUnknown() || vInfos->fileType.isDir())
		{
			fpn= vInfos->filePath + std::string(1u, PATH_SEP) + vInfos->fileNameExt;
		}

		struct stat           statInfos= {};
		std::array<char, 100> timebuf;
		int                   result= stat(fpn.c_str(), &statInfos);
		if(!result)
		{
			if(!vInfos->fileType.isDir())
			{
				vInfos->fileSize        = (size_t)statInfos.st_size;
				vInfos->formatedFileSize= prFormatFileSize(vInfos->fileSize);
			}

			size_t len= 0;
	#ifdef _MSC_VER
			struct tm _tm;
			errno_t   err= localtime_s(&_tm, &statInfos.st_mtime);
			if(!err)
			{
				len= strftime(timebuf.data(), 99, DateTimeFormat, &_tm);
			}
	#else  // _MSC_VER
			struct tm* _tm= localtime(&statInfos.st_mtime);
			if(_tm != nullptr)
			{
				len= strftime(timebuf.data(), 99, DateTimeFormat, _tm);
			}
	#endif // _MSC_VER
			if(len != 0u)
			{
				vInfos->fileModifDate= std::string(timebuf.data(), len);
			}
		}
	}
}

IGFD_API void IGFD::FileManager::prRemoveFileNameInSelection(const std::string& vFileName)
{
	prSelectedFileNames.erase(vFileName);

	if(prSelectedFileNames.size() == 1)
	{
		snprintf(puFileNameBuffer.data(), MAX_FILE_DIALOG_NAME_BUFFER, "%s", vFileName.c_str());
	}
	else
	{
		snprintf(
				puFileNameBuffer.data(), MAX_FILE_DIALOG_NAME_BUFFER, "%zu files Selected",
				prSelectedFileNames.size()
		);
	}
}

IGFD_API void IGFD::FileManager::prAddFileNameInSelection(
		const std::string& vFileName,
		bool               vSetLastSelectionFileName
)
{
	prSelectedFileNames.emplace(vFileName);

	if(prSelectedFileNames.size() == 1)
	{
		snprintf(puFileNameBuffer.data(), MAX_FILE_DIALOG_NAME_BUFFER, "%s", vFileName.c_str());
	}
	else
	{
		snprintf(
				puFileNameBuffer.data(), MAX_FILE_DIALOG_NAME_BUFFER, "%zu files Selected",
				prSelectedFileNames.size()
		);
	}

	if(vSetLastSelectionFileName)
	{
		prLastSelectedFileName= vFileName;
	}
}

IGFD_API void IGFD::FileManager::SetCurrentDir(const std::string& vPath)
{
	std::string path= vPath;
	#ifdef _IGFD_WIN_
	if(puFsRoot == path)
	{
		path+= std::string(1u, PATH_SEP);
	}
	#endif // _IGFD_WIN_

	namespace fs   = std::filesystem;
	bool dir_opened= fs::is_directory(vPath);
	if(!dir_opened)
	{
		path      = ".";
		dir_opened= fs::is_directory(vPath);
	}
	if(dir_opened)
	{
	#ifdef _IGFD_WIN_
		DWORD        numchar= 0;
		std::wstring wpath  = IGFD::Utils::UTF8Decode(path);
		numchar             = GetFullPathNameW(wpath.c_str(), 0, nullptr, nullptr);
		std::wstring fpath(numchar, 0);
		GetFullPathNameW(wpath.c_str(), numchar, (wchar_t*)fpath.data(), nullptr);
		std::string real_path= IGFD::Utils::UTF8Encode(fpath);
		while(real_path.back() == '\0'
		) // for fix issue we can have with std::string concatenation.. if there is a \0 at end
		{
			real_path= real_path.substr(0, real_path.size() - 1U);
		}
		if(!real_path.empty())
	#elif defined(_IGFD_UNIX_) // _IGFD_UNIX_ is _IGFD_WIN_ or APPLE
		std::array<char, PATH_MAX> real_path;

		char* numchar= realpath(path.c_str(), real_path.data());
		if(numchar != nullptr)
	#endif                     // _IGFD_WIN_
		{
			prCurrentPath= real_path.data();
			if(prCurrentPath[prCurrentPath.size() - 1] == PATH_SEP)
			{
				prCurrentPath= prCurrentPath.substr(0, prCurrentPath.size() - 1);
			}
			IGFD::Utils::SetBuffer(puInputPathBuffer.data(), MAX_PATH_BUFFER_SIZE, prCurrentPath);
			prCurrentPathDecomposition= IGFD::Utils::SplitStringToVector(prCurrentPath, PATH_SEP, false);
	#ifdef _IGFD_UNIX_ // _IGFD_UNIX_ is _IGFD_WIN_ or APPLE
			prCurrentPathDecomposition.insert(
					prCurrentPathDecomposition.begin(), std::string(1u, PATH_SEP)
			);
	#endif // _IGFD_UNIX_
			if(!prCurrentPathDecomposition.empty())
			{
	#ifdef _IGFD_WIN_
				puFsRoot= prCurrentPathDecomposition[0];
	#endif // _IGFD_WIN_
			}
		}
	}
}

IGFD_API bool IGFD::FileManager::CreateDir(const std::string& vPath)
{
	bool res= false;

	if(!vPath.empty())
	{
		std::string path= prCurrentPath + std::string(1u, PATH_SEP) + vPath;

		res= IGFD::Utils::CreateDirectoryIfNotExist(path);
	}

	return res;
}

IGFD_API std::string IGFD::FileManager::ComposeNewPath(std::vector<std::string>::iterator vIter)
{
	std::string res;

	while(true)
	{
		if(!res.empty())
		{
	#ifdef _IGFD_WIN_
			res= *vIter + std::string(1u, PATH_SEP) + res;
	#elif defined(_IGFD_UNIX_) // _IGFD_UNIX_ is _IGFD_WIN_ or APPLE
			if(*vIter == puFsRoot)
			{
				res= *vIter + res;
			}
			else
			{
				res= *vIter + PATH_SEP + res;
			}
	#endif                     // _IGFD_WIN_
		}
		else
		{
			res= *vIter;
		}

		if(vIter == prCurrentPathDecomposition.begin())
		{
	#ifdef _IGFD_UNIX_ // _IGFD_UNIX_ is _IGFD_WIN_ or APPLE
			if(res[0] != PATH_SEP)
			{
				res= PATH_SEP + res;
			}
	#endif // defined(_IGFD_UNIX_)
			break;
		}

		--vIter;
	}

	return res;
}

IGFD_API bool IGFD::FileManager::SetPathOnParentDirectoryIfAny()
{
	if(prCurrentPathDecomposition.size() > 1)
	{
		prCurrentPath= ComposeNewPath(prCurrentPathDecomposition.end() - 2);
		return true;
	}
	return false;
}

IGFD_API std::string IGFD::FileManager::GetCurrentPath()
{
	if(prCurrentPath.empty())
	{
		prCurrentPath= ".";
	}
	return prCurrentPath;
}

IGFD_API void IGFD::FileManager::SetCurrentPath(const std::string& vCurrentPath)
{
	if(vCurrentPath.empty())
	{
		prCurrentPath= ".";
	}
	else
	{
		prCurrentPath= vCurrentPath;
	}
}

IGFD_API bool IGFD::FileManager::IsFileExist(const std::string& vFile)
{
	std::ifstream docFile(vFile, std::ios::in);
	if(docFile.is_open())
	{
		docFile.close();
		return true;
	}
	return false;
}

IGFD_API void IGFD::FileManager::SetDefaultFileName(const std::string& vFileName)
{
	puDLGDefaultFileName= vFileName;
	IGFD::Utils::SetBuffer(puFileNameBuffer.data(), MAX_FILE_DIALOG_NAME_BUFFER, vFileName);
}

IGFD_API bool IGFD::FileManager::SelectDirectory(const std::shared_ptr<FileInfos>& vInfos)
{
	if(!vInfos.use_count())
	{
		return false;
	}

	bool pathClick= false;

	if(vInfos->fileNameExt == "..")
	{
		pathClick= SetPathOnParentDirectoryIfAny();
	}
	else
	{
		std::string newPath;

		if(puShowDrives)
		{
			newPath= vInfos->fileNameExt + std::string(1u, PATH_SEP);
		}
		else
		{
	#ifdef __linux__
			if(puFsRoot == prCurrentPath)
			{
				newPath= prCurrentPath + vInfos->fileNameExt;
			}
			else
	#endif // __linux__
				newPath= prCurrentPath + std::string(1u, PATH_SEP) + vInfos->fileNameExt;
		}

		if(IGFD::Utils::IsDirectoryCanBeOpened(newPath))
		{
			if(puShowDrives)
			{
				prCurrentPath= vInfos->fileNameExt;
				puFsRoot     = prCurrentPath;
			}
			else
			{
				prCurrentPath= newPath; //-V820
			}
			pathClick= true;
		}
	}

	return pathClick;
}

IGFD_API void IGFD::FileManager::SelectFileName(
		const FileDialogInternal&         vFileDialogInternal,
		const std::shared_ptr<FileInfos>& vInfos
)
{
	if(!vInfos.use_count())
	{
		return;
	}

	if(ImGui::IsKeyDown(ImGuiMod_Ctrl))
	{
		if(puDLGcountSelectionMax == 0) // infinite selection
		{
			if(prSelectedFileNames.find(vInfos->fileNameExt) ==
				 prSelectedFileNames.end()) // not found +> add
			{
				prAddFileNameInSelection(vInfos->fileNameExt, true);
			}
			else
			{ // found +> remove
				prRemoveFileNameInSelection(vInfos->fileNameExt);
			}
		}
		else // selection limited by size
		{
			if(prSelectedFileNames.size() < puDLGcountSelectionMax)
			{
				if(prSelectedFileNames.find(vInfos->fileNameExt) ==
					 prSelectedFileNames.end()) // not found +> add
				{
					prAddFileNameInSelection(vInfos->fileNameExt, true);
				}
				else
				{ // found +> remove
					prRemoveFileNameInSelection(vInfos->fileNameExt);
				}
			}
		}
	}
	else if(ImGui::IsKeyDown(ImGuiMod_Shift))
	{
		if(puDLGcountSelectionMax != 1)
		{
			prSelectedFileNames.clear();
			// we will iterate filelist and get the last selection after the start selection
			bool        startMultiSelection= false;
			std::string fileNameToSelect   = vInfos->fileNameExt;
			std::string savedLastSelectedFileName; // for invert selection mode
			for(const auto& file : prFileList)
			{
				if(!file.use_count())
				{
					continue;
				}

				bool canTake= true;
				if(!file->SearchForTag(vFileDialogInternal.puSearchManager.puSearchTag))
				{
					canTake= false;
				}
				if(canTake) // if not filtered, we will take files who are filtered by the dialog
				{
					if(file->fileNameExt == prLastSelectedFileName)
					{
						startMultiSelection= true;
						prAddFileNameInSelection(prLastSelectedFileName, false);
					}
					else if(startMultiSelection)
					{
						if(puDLGcountSelectionMax == 0) // infinite selection
						{
							prAddFileNameInSelection(file->fileNameExt, false);
						}
						else
						{ // selection limited by size
							if(prSelectedFileNames.size() < puDLGcountSelectionMax)
							{
								prAddFileNameInSelection(file->fileNameExt, false);
							}
							else
							{
								startMultiSelection= false;
								if(!savedLastSelectedFileName.empty())
								{
									prLastSelectedFileName= savedLastSelectedFileName;
								}
								break;
							}
						}
					}

					if(file->fileNameExt == fileNameToSelect)
					{
						if(!startMultiSelection) // we are before the last Selected FileName, so we must inverse
						{
							savedLastSelectedFileName= prLastSelectedFileName;
							prLastSelectedFileName   = fileNameToSelect;
							fileNameToSelect         = savedLastSelectedFileName;
							startMultiSelection      = true;
							prAddFileNameInSelection(prLastSelectedFileName, false);
						}
						else
						{
							startMultiSelection= false;
							if(!savedLastSelectedFileName.empty())
							{
								prLastSelectedFileName= savedLastSelectedFileName;
							}
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		prSelectedFileNames.clear();
		IGFD::Utils::ResetBuffer(puFileNameBuffer.data());
		prAddFileNameInSelection(vInfos->fileNameExt, true);
	}
}

IGFD_API void IGFD::FileManager::DrawDirectoryCreation(const FileDialogInternal& vFileDialogInternal
)
{
	if(vFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableCreateDirectoryButton)
	{
		return;
	}

	if(IMGUI_BUTTON(createDirButtonString))
	{
		if(!prCreateDirectoryMode)
		{
			prCreateDirectoryMode= true;
			IGFD::Utils::ResetBuffer(puDirectoryNameBuffer.data());
		}
	}
	if(ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(buttonCreateDirString);
	}

	if(prCreateDirectoryMode)
	{
		ImGui::SameLine();

		ImGui::PushItemWidth(100.0f);
		ImGui::InputText(
				"##DirectoryFileName", puDirectoryNameBuffer.data(), MAX_FILE_DIALOG_NAME_BUFFER
		);
		ImGui::PopItemWidth();

		ImGui::SameLine();

		if(IMGUI_BUTTON(okButtonString))
		{
			std::string newDir= std::string(puDirectoryNameBuffer.data());
			if(CreateDir(newDir))
			{
				SetCurrentPath(prCurrentPath + std::string(1u, PATH_SEP) + newDir);
				OpenCurrentPath(vFileDialogInternal);
			}

			prCreateDirectoryMode= false;
		}

		ImGui::SameLine();

		if(IMGUI_BUTTON(cancelButtonString))
		{
			prCreateDirectoryMode= false;
		}
	}

	ImGui::SameLine();
}

IGFD_API void IGFD::FileManager::DrawPathComposer(const FileDialogInternal& vFileDialogInternal)
{
	if(IMGUI_BUTTON(resetButtonString))
	{
		SetCurrentPath(".");
		OpenCurrentPath(vFileDialogInternal);
	}
	if(ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(buttonResetPathString);
	}

	#ifdef _IGFD_WIN_
	ImGui::SameLine();

	if(IMGUI_BUTTON(drivesButtonString))
	{
		puDrivesClicked= true;
	}
	if(ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(buttonDriveString);
	}
	#endif // _IGFD_WIN_

	ImGui::SameLine();

	if(IMGUI_BUTTON(editPathButtonString))
	{
		puInputPathActivated= !puInputPathActivated;
		if(puInputPathActivated)
		{
			auto endIt   = prCurrentPathDecomposition.end();
			prCurrentPath= ComposeNewPath(--endIt);
			IGFD::Utils::SetBuffer(puInputPathBuffer.data(), MAX_PATH_BUFFER_SIZE, prCurrentPath);
		}
	}
	if(ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(buttonEditPathString);
	}

	ImGui::SameLine();

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

	// show current path
	if(!prCurrentPathDecomposition.empty())
	{
		ImGui::SameLine();

		if(puInputPathActivated)
		{
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::InputText("##pathedition", puInputPathBuffer.data(), MAX_PATH_BUFFER_SIZE);
			ImGui::PopItemWidth();
		}
		else
		{
			int _id= 0;
			for(auto itPathDecomp= prCurrentPathDecomposition.begin();
					itPathDecomp != prCurrentPathDecomposition.end(); ++itPathDecomp)
			{
				if(itPathDecomp != prCurrentPathDecomposition.begin())
				{
	#if defined(CUSTOM_PATH_SPACING)
					ImGui::SameLine(0, CUSTOM_PATH_SPACING);
	#else
					ImGui::SameLine();
	#endif // USE_CUSTOM_PATH_SPACING
					if(!(vFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableQuickPathSelection))
					{
	#if defined(_IGFD_WIN_)
						const char* sep= "\\";
	#elif defined(_IGFD_UNIX_)
						const char* sep= "/";
						if(itPathDecomp != prCurrentPathDecomposition.begin() + 1)
	#endif
						{
							ImGui::PushID(_id++);
							bool click= IMGUI_PATH_BUTTON(sep);
							ImGui::PopID();

	#if defined(CUSTOM_PATH_SPACING)
							ImGui::SameLine(0, CUSTOM_PATH_SPACING);
	#else
							ImGui::SameLine();
	#endif // USE_CUSTOM_PATH_SPACING

							if(click)
							{
								OpenPathPopup(vFileDialogInternal, itPathDecomp - 1);
							}
							else if(ImGui::IsItemClicked(ImGuiMouseButton_Right))
							{
								SetCurrentPath(itPathDecomp - 1);
								break;
							}
						}
					}
				}

				ImGui::PushID(_id++);
				bool click= IMGUI_PATH_BUTTON((*itPathDecomp).c_str());
				ImGui::PopID();
				if(click)
				{
					prCurrentPath= ComposeNewPath(itPathDecomp);
					puPathClicked= true;
					break;
				}
				else if(ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{ // activate input for path
					SetCurrentPath(itPathDecomp);
					break;
				}
			}
		}
	}
}

IGFD_API void IGFD::FileManager::SetCurrentPath(std::vector<std::string>::iterator vPathIter)
{
	prCurrentPath= ComposeNewPath(vPathIter);
	IGFD::Utils::SetBuffer(puInputPathBuffer.data(), MAX_PATH_BUFFER_SIZE, prCurrentPath);
	puInputPathActivated= true;
}

IGFD_API std::string IGFD::FileManager::GetResultingPath()
{
	std::string path= prCurrentPath;
	if(puDLGDirectoryMode)
	{ // if directory mode
		std::string selectedDirectory(puFileNameBuffer.data());
		if(!selectedDirectory.empty() && selectedDirectory != ".")
		{
			path+= std::string(1u, PATH_SEP) + selectedDirectory;
		}
	}
	return path;
}

IGFD_API std::string IGFD::FileManager::GetResultingFileName(
		FileDialogInternal& vFileDialogInternal,
		IGFD_ResultMode     vFlag
)
{
	if(!puDLGDirectoryMode)
	{ // if not directory mode
		const auto& filename= std::string(puFileNameBuffer.data());
		return vFileDialogInternal.puFilterManager.ReplaceExtentionWithCurrentFilterIfNeeded(
				filename, vFlag
		);
	}
	return ""; // directory mode
}

IGFD_API std::string IGFD::FileManager::GetResultingFilePathName(
		FileDialogInternal& vFileDialogInternal,
		IGFD_ResultMode     vFlag
)
{
	auto        result  = GetResultingPath();
	const auto& filename= GetResultingFileName(vFileDialogInternal, vFlag);
	if(!filename.empty())
	{
	#ifdef _IGFD_UNIX_
		if(puFsRoot != result)
	#endif // _IGFD_UNIX_
		{
			result+= std::string(1u, PATH_SEP);
		}
		result+= filename;
	}

	return result;
}

IGFD_API std::map<std::string, std::string> IGFD::FileManager::GetResultingSelection(
		FileDialogInternal& vFileDialogInternal,
		IGFD_ResultMode     vFlag
)
{
	std::map<std::string, std::string> res;
	for(const auto& selectedFileName : prSelectedFileNames)
	{
		auto result= GetResultingPath();
	#ifdef _IGFD_UNIX_
		if(puFsRoot != result)
	#endif // _IGFD_UNIX_
		{
			result+= std::string(1u, PATH_SEP);
		}

		result+= vFileDialogInternal.puFilterManager.ReplaceExtentionWithCurrentFilterIfNeeded(
				selectedFileName, vFlag
		);
		res[selectedFileName]= result;
	}
	return res;
}

	#pragma endregion

	#pragma region FileDialogInternal

IGFD_API void IGFD::FileDialogInternal::NewFrame()
{
	puCanWeContinue              = true;  // reset flag for possibily validate the dialog
	puIsOk                       = false; // reset dialog result
	puFileManager.puDrivesClicked= false;
	puFileManager.puPathClicked  = false;

	puNeedToExitDialog= false;

	#ifdef USE_DIALOG_EXIT_WITH_KEY
	if(ImGui::IsKeyPressed(IGFD_EXIT_KEY))
	{
		// we do that here with the data's defined at the last frame
		// because escape key can quit input activation and at the end of the frame all flag will be
		// false so we will detect nothing
		if(!(puFileManager.puInputPathActivated || puSearchManager.puSearchInputIsActive ||
				 puFileInputIsActive || puFileListViewIsActive))
		{
			puNeedToExitDialog= true; // need to quit dialog
		}
	}
	else
	#endif
	{
		puSearchManager.puSearchInputIsActive= false;
		puFileInputIsActive                  = false;
		puFileListViewIsActive               = false;
	}
}

IGFD_API void IGFD::FileDialogInternal::EndFrame()
{
	// directory change
	if(puFileManager.puPathClicked)
	{
		puFileManager.OpenCurrentPath(*this);
	}

	if(puFileManager.puDrivesClicked)
	{
		if(puFileManager.GetDrives())
		{
			puFileManager.ApplyFilteringOnFileList(*this);
		}
	}

	if(puFileManager.puInputPathActivated)
	{
		auto gio= ImGui::GetIO();
		if(ImGui::IsKeyReleased(ImGuiKey_Enter))
		{
			puFileManager.SetCurrentPath(std::string(puFileManager.puInputPathBuffer.data()));
			puFileManager.OpenCurrentPath(*this);
			puFileManager.puInputPathActivated= false;
		}
		if(ImGui::IsKeyReleased(ImGuiKey_Escape))
		{
			puFileManager.puInputPathActivated= false;
		}
	}
}

IGFD_API void IGFD::FileDialogInternal::ResetForNewDialog() { }

	#pragma endregion

	#pragma endregion

	#pragma region Optional Features

	#pragma region ThumbnailFeature

IGFD_API IGFD::ThumbnailFeature::ThumbnailFeature()
{
	#ifdef USE_THUMBNAILS
	prDisplayMode= DisplayModeEnum::FILE_LIST;
	#endif
}

IGFD_API IGFD::ThumbnailFeature::~ThumbnailFeature()= default;

IGFD_API void IGFD::ThumbnailFeature::NewThumbnailFrame(FileDialogInternal& /*vFileDialogInternal*/)
{
	#ifdef USE_THUMBNAILS
	prStartThumbnailFileDatasExtraction();
	#endif
}

IGFD_API void IGFD::ThumbnailFeature::EndThumbnailFrame(FileDialogInternal& vFileDialogInternal)
{
	#ifdef USE_THUMBNAILS
	prClearThumbnails(vFileDialogInternal);
	#else
	(void)vFileDialogInternal;
	#endif
}

IGFD_API void IGFD::ThumbnailFeature::QuitThumbnailFrame(FileDialogInternal& vFileDialogInternal)
{
	#ifdef USE_THUMBNAILS
	prStopThumbnailFileDatasExtraction();
	prClearThumbnails(vFileDialogInternal);
	#else
	(void)vFileDialogInternal;
	#endif
}

	#ifdef USE_THUMBNAILS
IGFD_API void IGFD::ThumbnailFeature::prStartThumbnailFileDatasExtraction()
{
	const bool res=
			prThumbnailGenerationThread.use_count() && prThumbnailGenerationThread->joinable();
	if(!res)
	{
		prIsWorking                = true;
		prCountFiles               = 0U;
		prThumbnailGenerationThread= std::shared_ptr<std::thread>(
				new std::thread(&IGFD::ThumbnailFeature::prThreadThumbnailFileDatasExtractionFunc, this),
				[this](std::thread* obj)
				{
					prIsWorking= false;
					if(obj)
					{
						obj->join();
					}
				}
		);
	}
}

IGFD_API bool IGFD::ThumbnailFeature::prStopThumbnailFileDatasExtraction()
{
	const bool res=
			prThumbnailGenerationThread.use_count() && prThumbnailGenerationThread->joinable();
	if(res)
	{
		prThumbnailGenerationThread.reset();
	}

	return res;
}

IGFD_API void IGFD::ThumbnailFeature::prThreadThumbnailFileDatasExtractionFunc()
{
	prCountFiles= 0U;
	prIsWorking = true;

	// infinite loop while is thread working
	while(prIsWorking)
	{
		if(!prThumbnailFileDatasToGet.empty())
		{
			std::shared_ptr<FileInfos> file= nullptr;
			prThumbnailFileDatasToGetMutex.lock();
			// get the first file in the list
			file= (*prThumbnailFileDatasToGet.begin());
			prThumbnailFileDatasToGetMutex.unlock();

			// retrieve datas of the texture file if its an image file
			if(file.use_count())
			{
				if(file->fileType.isFile()) //-V522
				{
					//|| file->fileExtLevels == ".hdr" => format float so in few times
					if(file->SearchForExts(".png,.bmp,.tga,.jpg,.jpeg,.gif,.psd,.pic,.ppm,.pgm", true))
					{
						auto fpn= file->filePath + std::string(1u, PATH_SEP) + file->fileNameExt;

						int      w    = 0;
						int      h    = 0;
						int      chans= 0;
						uint8_t* datas= stbi_load(fpn.c_str(), &w, &h, &chans, STBI_rgb_alpha);
						if(datas != nullptr)
						{
							if((w != 0) && (h != 0))
							{
								// resize with respect to glyph ratio
								const float ratioX= (float)w / (float)h;
								const float newX  = DisplayMode_ThumbailsList_ImageHeight * ratioX;
								float       newY  = w / ratioX;
								if(newX < w)
								{
									newY= DisplayMode_ThumbailsList_ImageHeight;
								}

								const auto newWidth   = (int)newX;
								const auto newHeight  = (int)newY;
								const auto newBufSize = (size_t)(newWidth * newHeight * 4U); //-V112 //-V1028
								uint8_t*   resizedData= new uint8_t[newBufSize];

								const int resizeSucceeded= stbir_resize_uint8(
										datas, w, h, 0, resizedData, newWidth, newHeight, 0,
										4
								); //-V112

								if(resizeSucceeded)
								{
									auto th= &file->thumbnailInfo;

									th->textureFileDatas= resizedData;
									th->textureWidth    = newWidth;
									th->textureHeight   = newHeight;
									th->textureChannels = 4; //-V112

									// we set that at least, because will launch the gpu creation of the texture in
									// the main thread
									th->isReadyToUpload= true;

									// need gpu loading
									prAddThumbnailToCreate(file);
								}
							}
							else
							{
								printf("image loading fail : w:%i h:%i c:%i\n", w, h, 4); //-V112
							}

							stbi_image_free(datas);
						}
					}
				}

				// peu importe le resultat on vire le fichier
				// remove form this list
				// write => thread concurency issues
				prThumbnailFileDatasToGetMutex.lock();
				prThumbnailFileDatasToGet.pop_front();
				prThumbnailFileDatasToGetMutex.unlock();
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

IGFD_API void IGFD::ThumbnailFeature::prVariadicProgressBar(
		float         fraction,
		const ImVec2& size_arg,
		const char*   fmt,
		...
)
{
	va_list args{};
	va_start(args, fmt);
	std::array<char, 512> TempBuffer{};
	const int             w= vsnprintf(TempBuffer.data(), 511, fmt, args);
	va_end(args);
	if(w != 0)
	{
		ImGui::ProgressBar(fraction, size_arg, TempBuffer.data());
	}
}

IGFD_API void IGFD::ThumbnailFeature::prDrawThumbnailGenerationProgress()
{
	if(prThumbnailGenerationThread.use_count() && prThumbnailGenerationThread->joinable())
	{
		if(!prThumbnailFileDatasToGet.empty())
		{
			const auto p= (float)((double)prCountFiles / (double)prThumbnailFileDatasToGet.size()
			); // read => no thread concurency issues
			prVariadicProgressBar(
					p, ImVec2(50, 0), "%u/%u", prCountFiles,
					(uint32_t)prThumbnailFileDatasToGet.size()
			); // read => no thread concurency issues
			ImGui::SameLine();
		}
	}
}

IGFD_API void
IGFD::ThumbnailFeature::prAddThumbnailToLoad(const std::shared_ptr<FileInfos>& vFileInfos)
{
	if(vFileInfos.use_count())
	{
		if(vFileInfos->fileType.isFile())
		{
			//|| file->fileExtLevels == ".hdr" => format float so in few times
			if(vFileInfos->SearchForExts(".png,.bmp,.tga,.jpg,.jpeg,.gif,.psd,.pic,.ppm,.pgm", true))
			{
				// write => thread concurency issues
				prThumbnailFileDatasToGetMutex.lock();
				prThumbnailFileDatasToGet.push_back(vFileInfos);
				vFileInfos->thumbnailInfo.isLoadingOrLoaded= true;
				prThumbnailFileDatasToGetMutex.unlock();
			}
		}
	}
}

IGFD_API void
IGFD::ThumbnailFeature::prAddThumbnailToCreate(const std::shared_ptr<FileInfos>& vFileInfos)
{
	if(vFileInfos.use_count())
	{
		// write => thread concurency issues
		prThumbnailToCreateMutex.lock();
		prThumbnailToCreate.push_back(vFileInfos);
		prThumbnailToCreateMutex.unlock();
	}
}

IGFD_API void
IGFD::ThumbnailFeature::prAddThumbnailToDestroy(const IGFD_Thumbnail_Info& vIGFD_Thumbnail_Info)
{
	// write => thread concurency issues
	prThumbnailToDestroyMutex.lock();
	prThumbnailToDestroy.push_back(vIGFD_Thumbnail_Info);
	prThumbnailToDestroyMutex.unlock();
}

IGFD_API void IGFD::ThumbnailFeature::prDrawDisplayModeToolBar()
{
	if(IMGUI_RADIO_BUTTON(
				 DisplayMode_FilesList_ButtonString, prDisplayMode == DisplayModeEnum::FILE_LIST
		 ))
	{
		prDisplayMode= DisplayModeEnum::FILE_LIST;
	}
	if(ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(DisplayMode_FilesList_ButtonHelp);
	}
	ImGui::SameLine();
	if(IMGUI_RADIO_BUTTON(
				 DisplayMode_ThumbailsList_ButtonString, prDisplayMode == DisplayModeEnum::THUMBNAILS_LIST
		 ))
	{
		prDisplayMode= DisplayModeEnum::THUMBNAILS_LIST;
	}
	if(ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(DisplayMode_ThumbailsList_ButtonHelp);
	}
	ImGui::SameLine();
	/* todo
	if (IMGUI_RADIO_BUTTON(DisplayMode_ThumbailsGrid_ButtonString,
			prDisplayMode == DisplayModeEnum::THUMBNAILS_GRID))
			prDisplayMode = DisplayModeEnum::THUMBNAILS_GRID;
	if (ImGui::IsItemHovered())	ImGui::SetTooltip(DisplayMode_ThumbailsGrid_ButtonHelp);
	ImGui::SameLine();
	*/
	prDrawThumbnailGenerationProgress();
}

IGFD_API void IGFD::ThumbnailFeature::prClearThumbnails(FileDialogInternal& vFileDialogInternal)
{
	// directory wil be changed so the file list will be erased
	if(vFileDialogInternal.puFileManager.puPathClicked)
	{
		size_t count= vFileDialogInternal.puFileManager.GetFullFileListSize();
		for(size_t idx= 0U; idx < count; idx++)
		{
			auto file= vFileDialogInternal.puFileManager.GetFullFileAt(idx);
			if(file.use_count())
			{
				if(file->thumbnailInfo.isReadyToDisplay) //-V522
				{
					prAddThumbnailToDestroy(file->thumbnailInfo);
				}
			}
		}
	}
}

IGFD_API void
IGFD::ThumbnailFeature::SetCreateThumbnailCallback(const CreateThumbnailFun& vCreateThumbnailFun)
{
	prCreateThumbnailFun= vCreateThumbnailFun;
}

IGFD_API void
IGFD::ThumbnailFeature::SetDestroyThumbnailCallback(const DestroyThumbnailFun& vCreateThumbnailFun)
{
	prDestroyThumbnailFun= vCreateThumbnailFun;
}

IGFD_API void IGFD::ThumbnailFeature::ManageGPUThumbnails()
{
	if(prCreateThumbnailFun)
	{
		if(!prThumbnailToCreate.empty())
		{
			for(const auto& file : prThumbnailToCreate)
			{
				if(file.use_count())
				{
					prCreateThumbnailFun(&file->thumbnailInfo);
				}
			}
			prThumbnailToCreateMutex.lock();
			prThumbnailToCreate.clear();
			prThumbnailToCreateMutex.unlock();
		}
	}
	else
	{
		printf("No Callback found for create texture\nYou need to define the callback with a call to "
					 "SetCreateThumbnailCallback\n");
	}

	if(prDestroyThumbnailFun)
	{
		if(!prThumbnailToDestroy.empty())
		{
			for(auto thumbnail : prThumbnailToDestroy)
			{
				prDestroyThumbnailFun(&thumbnail);
			}
			prThumbnailToDestroyMutex.lock();
			prThumbnailToDestroy.clear();
			prThumbnailToDestroyMutex.unlock();
		}
	}
	else
	{
		printf("No Callback found for destroy texture\nYou need to define the callback with a call to "
					 "SetCreateThumbnailCallback\n");
	}
}

	#endif // USE_THUMBNAILS

	#pragma endregion

	#pragma region BookMarkFeature

IGFD_API IGFD::BookMarkFeature::BookMarkFeature()
{
	#ifdef USE_BOOKMARK
	prBookmarkWidth= defaultBookmarkPaneWith;
	#endif // USE_BOOKMARK
}

	#ifdef USE_BOOKMARK
IGFD_API void IGFD::BookMarkFeature::prDrawBookmarkButton()
{
	IMGUI_TOGGLE_BUTTON(bookmarksButtonString, &prBookmarkPaneShown);

	if(ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(bookmarksButtonHelpString);
	}
}

IGFD_API bool IGFD::BookMarkFeature::prDrawBookmarkPane(
		FileDialogInternal& vFileDialogInternal,
		const ImVec2&       vSize
)
{
	bool res= false;

	ImGui::BeginChild("##bookmarkpane", vSize);

	static int selectedBookmarkForEdition= -1;

	if(IMGUI_BUTTON(addBookmarkButtonString "##ImGuiFileDialogAddBookmark"))
	{
		if(!vFileDialogInternal.puFileManager.IsComposerEmpty())
		{
			BookmarkStruct bookmark;
			bookmark.name= vFileDialogInternal.puFileManager.GetBack();
			bookmark.path= vFileDialogInternal.puFileManager.GetCurrentPath();
			prBookmarks.push_back(bookmark);
		}
	}
	if(selectedBookmarkForEdition >= 0 && selectedBookmarkForEdition < (int)prBookmarks.size())
	{
		ImGui::SameLine();
		if(IMGUI_BUTTON(removeBookmarkButtonString "##ImGuiFileDialogAddBookmark"))
		{
			prBookmarks.erase(prBookmarks.begin() + selectedBookmarkForEdition);
			if(selectedBookmarkForEdition == (int)prBookmarks.size())
			{
				selectedBookmarkForEdition--;
			}
		}

		if(selectedBookmarkForEdition >= 0 && selectedBookmarkForEdition < (int)prBookmarks.size())
		{
			ImGui::SameLine();

			ImGui::PushItemWidth(vSize.x - ImGui::GetCursorPosX());
			if(ImGui::InputText(
						 "##ImGuiFileDialogBookmarkEdit", prBookmarkEditBuffer, MAX_FILE_DIALOG_NAME_BUFFER
				 ))
			{
				prBookmarks[(size_t)selectedBookmarkForEdition].name= std::string(prBookmarkEditBuffer);
			}
			ImGui::PopItemWidth();
		}
	}

	ImGui::Separator();

	if(!prBookmarks.empty())
	{
		prBookmarkClipper.Begin((int)prBookmarks.size(), ImGui::GetTextLineHeightWithSpacing());
		while(prBookmarkClipper.Step())
		{
			for(int i= prBookmarkClipper.DisplayStart; i < prBookmarkClipper.DisplayEnd; i++)
			{
				if(i < 0)
				{
					continue;
				}
				const BookmarkStruct& bookmark= prBookmarks[(size_t)i];
				ImGui::PushID(i);
				if(ImGui::Selectable(
							 bookmark.name.c_str(), selectedBookmarkForEdition == i,
							 ImGuiSelectableFlags_AllowDoubleClick
					 ) ||
					 (selectedBookmarkForEdition == -1 &&
						bookmark.path == vFileDialogInternal.puFileManager.GetCurrentPath()
					 )) // select if path is current
				{
					selectedBookmarkForEdition= i;
					IGFD::Utils::ResetBuffer(prBookmarkEditBuffer);
					IGFD::Utils::AppendToBuffer(
							prBookmarkEditBuffer, MAX_FILE_DIALOG_NAME_BUFFER, bookmark.name
					);

					if(ImGui::IsMouseDoubleClicked(0)) // apply path
					{
						vFileDialogInternal.puFileManager.SetCurrentPath(bookmark.path);
						vFileDialogInternal.puFileManager.OpenCurrentPath(vFileDialogInternal);
						res= true;
					}
				}
				ImGui::PopID();
				if(ImGui::IsItemHovered())
				{
					ImGui::SetTooltip("%s", bookmark.path.c_str()); //-V111
				}
			}
		}
		prBookmarkClipper.End();
	}

	ImGui::EndChild();

	return res;
}

IGFD_API std::string
				 IGFD::BookMarkFeature::SerializeBookmarks(const bool& vDontSerializeCodeBasedBookmarks)
{
	std::string res;

	size_t idx= 0;
	for(auto& it : prBookmarks)
	{
		if(vDontSerializeCodeBasedBookmarks && it.defined_by_code)
		{
			continue;
		}

		if(idx++ != 0)
		{
			res+= "##"; // ## because reserved by imgui, so an input text cant have ##
		}

		res+= it.name + "##" + it.path;
	}

	return res;
}

IGFD_API void IGFD::BookMarkFeature::DeserializeBookmarks(const std::string& vBookmarks)
{
	if(!vBookmarks.empty())
	{
		prBookmarks.clear();
		auto arr= IGFD::Utils::SplitStringToVector(vBookmarks, '#', false);
		for(size_t i= 0; i < arr.size(); i+= 2)
		{
			if(i + 1 <
				 arr.size()) // for avoid crash if arr size is impair due to user mistake after edition
			{
				BookmarkStruct bookmark;
				bookmark.name= arr[i];
				// if bad format we jump this bookmark
				bookmark.path= arr[i + 1];
				prBookmarks.push_back(bookmark);
			}
		}
	}
}

IGFD_API void IGFD::BookMarkFeature::AddBookmark(
		const std::string& vBookMarkName,
		const std::string& vBookMarkPath
)
{
	if(vBookMarkName.empty() || vBookMarkPath.empty())
	{
		return;
	}

	BookmarkStruct bookmark;
	bookmark.name           = vBookMarkName;
	bookmark.path           = vBookMarkPath;
	bookmark.defined_by_code= true;
	prBookmarks.push_back(bookmark);
}

IGFD_API bool IGFD::BookMarkFeature::RemoveBookmark(const std::string& vBookMarkName)
{
	if(vBookMarkName.empty())
	{
		return false;
	}

	for(auto bookmark_it= prBookmarks.begin(); bookmark_it != prBookmarks.end(); ++bookmark_it)
	{
		if((*bookmark_it).name == vBookMarkName)
		{
			prBookmarks.erase(bookmark_it);
			return true;
		}
	}

	return false;
}
	#endif // USE_BOOKMARK

	#pragma endregion

	#pragma region KeyExplorerFeature

IGFD::KeyExplorerFeature::KeyExplorerFeature()= default;

	#ifdef USE_EXPLORATION_BY_KEYS
IGFD_API bool
IGFD::KeyExplorerFeature::prLocateItem_Loop(FileDialogInternal& vFileDialogInternal, ImWchar vC)
{
	bool found= false;

	auto& fdi= vFileDialogInternal.puFileManager;
	if(!fdi.IsFilteredListEmpty())
	{
		auto countFiles= fdi.GetFilteredListSize();
		for(size_t i= prLocateFileByInputChar_lastFileIdx; i < countFiles; i++)
		{
			auto nfo= fdi.GetFilteredFileAt(i);
			if(nfo.use_count())
			{
				if(nfo->fileNameExt_optimized[0] == vC || // lower case search //-V522
					 nfo->fileNameExt[0] == vC)             // maybe upper case search
				{
					// float p = ((float)i) * ImGui::GetTextLineHeightWithSpacing();
					float p= (float)((double)i / (double)countFiles) * ImGui::GetScrollMaxY();
					ImGui::SetScrollY(p);
					prLocateFileByInputChar_lastFound  = true;
					prLocateFileByInputChar_lastFileIdx= i;
					prStartFlashItem(prLocateFileByInputChar_lastFileIdx);

					auto infos= fdi.GetFilteredFileAt(prLocateFileByInputChar_lastFileIdx);
					if(infos.use_count())
					{
						if(infos->fileType.isDir()) //-V522
						{
							if(fdi.puDLGDirectoryMode) // directory chooser
							{
								fdi.SelectFileName(vFileDialogInternal, infos);
							}
						}
						else
						{
							fdi.SelectFileName(vFileDialogInternal, infos);
						}

						found= true;
						break;
					}
				}
			}
		}
	}

	return found;
}

IGFD_API void IGFD::KeyExplorerFeature::prLocateByInputKey(FileDialogInternal& vFileDialogInternal)
{
	ImGuiContext& g  = *GImGui;
	auto&         fdi= vFileDialogInternal.puFileManager;
	if(!g.ActiveId && !fdi.IsFilteredListEmpty())
	{
		auto& queueChar = ImGui::GetIO().InputQueueCharacters;
		auto  countFiles= fdi.GetFilteredListSize();

		// point by char
		if(!queueChar.empty())
		{
			ImWchar c= queueChar.back();
			if(prLocateFileByInputChar_InputQueueCharactersSize != queueChar.size())
			{
				if(c == prLocateFileByInputChar_lastChar) // next file starting with same char until
				{
					if(prLocateFileByInputChar_lastFileIdx < countFiles - 1U)
					{
						prLocateFileByInputChar_lastFileIdx++;
					}
					else
					{
						prLocateFileByInputChar_lastFileIdx= 0;
					}
				}

				if(!prLocateItem_Loop(vFileDialogInternal, c))
				{
					// not found, loop again from 0 this time
					prLocateFileByInputChar_lastFileIdx= 0;
					prLocateItem_Loop(vFileDialogInternal, c);
				}

				prLocateFileByInputChar_lastChar= c;
			}
		}

		prLocateFileByInputChar_InputQueueCharactersSize= queueChar.size();
	}
}

IGFD_API void IGFD::KeyExplorerFeature::prExploreWithkeys(
		FileDialogInternal& vFileDialogInternal,
		ImGuiID             vListViewID
)
{
	auto& fdi= vFileDialogInternal.puFileManager;
	if(!fdi.IsFilteredListEmpty())
	{
		bool canWeExplore= false;
		bool hasNav      = (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard);

		ImGuiContext& g= *GImGui;
		if(!hasNav && !g.ActiveId) // no nav and no activated inputs
		{
			canWeExplore= true;
		}

		if(g.NavId && g.NavId == vListViewID)
		{
			if(ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter) ||
				 ImGui::IsKeyPressed(ImGuiKey_Space))
			{
				ImGui::ActivateItemByID(vListViewID);
				ImGui::SetActiveID(vListViewID, g.CurrentWindow);
			}
		}

		if(vListViewID ==
			 g.LastActiveId -
					 1) // if listview id is the last acticated nav id (ImGui::ActivateItemByID(vListViewID);)
		{
			canWeExplore= true;
		}

		if(canWeExplore && ImGui::IsWindowFocused())
		{
			if(ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				ImGui::ClearActiveID();
				g.LastActiveId= 0;
			}

			auto countFiles= fdi.GetFilteredListSize();

			// explore
			bool exploreByKey    = false;
			bool enterInDirectory= false;
			bool exitDirectory   = false;

			if((hasNav && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) ||
				 (!hasNav && ImGui::IsKeyPressed(ImGuiKey_UpArrow)))
			{
				exploreByKey= true;
				if(prLocateFileByInputChar_lastFileIdx > 0)
				{
					prLocateFileByInputChar_lastFileIdx--;
				}
				else
				{
					prLocateFileByInputChar_lastFileIdx= countFiles - 1U;
				}
			}
			else if((hasNav && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) || (!hasNav && ImGui::IsKeyPressed(ImGuiKey_DownArrow)))
			{
				exploreByKey= true;
				if(prLocateFileByInputChar_lastFileIdx < countFiles - 1U)
				{
					prLocateFileByInputChar_lastFileIdx++;
				}
				else
				{
					prLocateFileByInputChar_lastFileIdx= 0U;
				}
			}
			else if(ImGui::IsKeyReleased(ImGuiKey_Enter))
			{
				exploreByKey    = true;
				enterInDirectory= true;
			}
			else if(ImGui::IsKeyReleased(ImGuiKey_Backspace))
			{
				exploreByKey = true;
				exitDirectory= true;
			}

			if(exploreByKey)
			{
				// float totalHeight = prFilteredFileList.size() * ImGui::GetTextLineHeightWithSpacing();
				float p= (float)((double)prLocateFileByInputChar_lastFileIdx / (double)(countFiles - 1U)) *
								 ImGui::GetScrollMaxY(); // seems not udpated in tables version outside tables
				// float p = ((float)locateFileByInputChar_lastFileIdx) *
				// ImGui::GetTextLineHeightWithSpacing();
				ImGui::SetScrollY(p);
				prStartFlashItem(prLocateFileByInputChar_lastFileIdx);

				auto infos= fdi.GetFilteredFileAt(prLocateFileByInputChar_lastFileIdx);
				if(infos.use_count())
				{
					if(infos->fileType.isDir()) //-V522
					{
						if(!fdi.puDLGDirectoryMode || enterInDirectory)
						{
							if(enterInDirectory)
							{
								if(fdi.SelectDirectory(infos))
								{
									// changement de repertoire
									vFileDialogInternal.puFileManager.OpenCurrentPath(vFileDialogInternal);
									if(prLocateFileByInputChar_lastFileIdx > countFiles - 1U)
									{
										prLocateFileByInputChar_lastFileIdx= 0;
									}
								}
							}
						}
						else // directory chooser
						{
							fdi.SelectFileName(vFileDialogInternal, infos);
						}
					}
					else
					{
						fdi.SelectFileName(vFileDialogInternal, infos);

						if(enterInDirectory)
						{
							vFileDialogInternal.puIsOk= true;
						}
					}

					if(exitDirectory)
					{
						auto nfo        = std::make_shared<FileInfos>();
						nfo->fileNameExt= "..";

						if(fdi.SelectDirectory(nfo))
						{
							// changement de repertoire
							vFileDialogInternal.puFileManager.OpenCurrentPath(vFileDialogInternal);
							if(prLocateFileByInputChar_lastFileIdx > countFiles - 1U)
							{
								prLocateFileByInputChar_lastFileIdx= 0;
							}
						}
		#ifdef _IGFD_WIN_
						else
						{
							if(fdi.GetComposerSize() == 1U)
							{
								if(fdi.GetDrives())
								{
									fdi.ApplyFilteringOnFileList(vFileDialogInternal);
								}
							}
						}
		#endif // _IGFD_WIN_
					}
				}
			}
		}
	}
}

IGFD_API bool IGFD::KeyExplorerFeature::prFlashableSelectable(
		const char*          label,
		bool                 selected,
		ImGuiSelectableFlags flags,
		bool                 vFlashing,
		const ImVec2&        size_arg
)
{
	using namespace ImGui;

	ImGuiWindow* window= GetCurrentWindow();
	if(window->SkipItems)
	{
		return false;
	}

	ImGuiContext&     g    = *GImGui;
	const ImGuiStyle& style= g.Style;

	// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning
	// rectangle.
	ImGuiID id        = window->GetID(label);
	ImVec2  label_size= CalcTextSize(label, NULL, true);
	ImVec2  size(
      size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y
	);
	ImVec2 pos= window->DC.CursorPos;
	pos.y+= window->DC.CurrLineTextBaseOffset;
	ItemSize(size, 0.0f);

	// Fill horizontal space
	// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make
	// explicitly right-aligned sizes not visibly match other widgets.
	const bool  span_all_columns= (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
	const float min_x           = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
	const float max_x= span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
	if(size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
	{
		size.x= ImMax(label_size.x, max_x - min_x);
	}

	// Text stays at the submission position, but bounding box may be extended on both sides
	const ImVec2 text_min= pos;
	const ImVec2 text_max(min_x + size.x, pos.y + size.y);

	// Selectables are meant to be tightly packed together with no click-gap, so we extend their box
	// to cover spacing between selectable.
	ImRect bb(min_x, pos.y, text_max.x, text_max.y);
	if((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
	{
		const float spacing_x= span_all_columns ? 0.0f : style.ItemSpacing.x;
		const float spacing_y= style.ItemSpacing.y;
		const float spacing_L= IM_TRUNC(spacing_x * 0.50f);
		const float spacing_U= IM_TRUNC(spacing_y * 0.50f);
		bb.Min.x-= spacing_L;
		bb.Min.y-= spacing_U;
		bb.Max.x+= (spacing_x - spacing_L);
		bb.Max.y+= (spacing_y - spacing_U);
	}
	// if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255));
	// }

	// Modify ClipRect for the ItemAdd(), faster than doing a
	// PushColumnsBackground/PushTableBackground for every Selectable..
	const float backup_clip_rect_min_x= window->ClipRect.Min.x;
	const float backup_clip_rect_max_x= window->ClipRect.Max.x;
	if(span_all_columns)
	{
		window->ClipRect.Min.x= window->ParentWorkRect.Min.x;
		window->ClipRect.Max.x= window->ParentWorkRect.Max.x;
	}

	const bool disabled_item= (flags & ImGuiSelectableFlags_Disabled) != 0;
	const bool item_add=
			ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
	if(span_all_columns)
	{
		window->ClipRect.Min.x= backup_clip_rect_min_x;
		window->ClipRect.Max.x= backup_clip_rect_max_x;
	}

	if(!item_add)
	{
		return false;
	}

	const bool disabled_global= (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
	if(disabled_item && !disabled_global) // Only testing this as an optimization
	{
		BeginDisabled();
	}

	// FIXME: We can standardize the behavior of those two, we could also keep the fast path of
	// override ClipRect + full push on render only, which would be advantageous since most selectable
	// are not selected.
	if(span_all_columns && window->DC.CurrentColumns)
	{
		PushColumnsBackground();
	}
	else if(span_all_columns && g.CurrentTable)
	{
		TablePushBackgroundChannel();
	}

	// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse
	// child entries
	ImGuiButtonFlags button_flags= 0;
	if(flags & ImGuiSelectableFlags_NoHoldingActiveID)
	{
		button_flags|= ImGuiButtonFlags_NoHoldingActiveId;
	}
	if(flags & ImGuiSelectableFlags_NoSetKeyOwner)
	{
		button_flags|= ImGuiButtonFlags_NoSetKeyOwner;
	}
	if(flags & ImGuiSelectableFlags_SelectOnClick)
	{
		button_flags|= ImGuiButtonFlags_PressedOnClick;
	}
	if(flags & ImGuiSelectableFlags_SelectOnRelease)
	{
		button_flags|= ImGuiButtonFlags_PressedOnRelease;
	}
	if(flags & ImGuiSelectableFlags_AllowDoubleClick)
	{
		button_flags|= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
	}
	if((flags & ImGuiSelectableFlags_AllowOverlap) ||
		 (g.LastItemData.InFlags & ImGuiItemFlags_AllowOverlap))
	{
		button_flags|= ImGuiButtonFlags_AllowOverlap;
	}

	const bool was_selected= selected;
	bool       hovered, held;
	bool       pressed= ButtonBehavior(bb, id, &hovered, &held, button_flags);

	// Auto-select when moved into
	// - This will be more fully fleshed in the range-select branch
	// - This is not exposed as it won't nicely work with some user side handling of shift/control
	// - We cannot do 'if (g.NavJustMovedToId != id) { selected = false; pressed = was_selected; }'
	// for two reasons
	//   - (1) it would require focus scope to be set, need exposing PushFocusScope() or equivalent
	//   (e.g. BeginSelection() calling PushFocusScope())
	//   - (2) usage will fail with clipped items
	//   The multi-select API aim to fix those issues, e.g. may be replaced with a BeginSelection()
	//   API.
	if((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 &&
		 g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId)
	{
		if(g.NavJustMovedToId == id)
		{
			selected= pressed= true;
		}
	}

	// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so
	// navigation can be resumed with gamepad/keyboard
	if(pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
	{
		if(!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
		{
			SetNavID(
					id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, WindowRectAbsToRel(window, bb)
			); // (bb == NavRect)
			g.NavDisableHighlight= true;
		}
	}
	if(pressed)
	{
		MarkItemEdited(id);
	}

	// In this branch, Selectable() cannot toggle the selection so this will never trigger.
	if(selected != was_selected) //-V547
	{
		g.LastItemData.StatusFlags|= ImGuiItemStatusFlags_ToggledSelection;
	}

	//////////////////////////////////////////////////////////////////
	// this function copy ImGui::Selectable just for this line....
	hovered|= vFlashing;
	//////////////////////////////////////////////////////////////////

	// Render
	if(hovered || selected)
	{
		const ImU32 col= GetColorU32(
				(held && hovered) ? ImGuiCol_HeaderActive
				: hovered         ? ImGuiCol_HeaderHovered
													: ImGuiCol_Header
		);
		RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
	}
	if(g.NavId == id)
	{
		RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
	}

	if(span_all_columns && window->DC.CurrentColumns)
	{
		PopColumnsBackground();
	}
	else if(span_all_columns && g.CurrentTable)
	{
		TablePopBackgroundChannel();
	}

	RenderTextClipped(text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);

	// Automatically close popups
	if(pressed && (window->Flags & ImGuiWindowFlags_Popup) &&
		 !(flags & ImGuiSelectableFlags_DontClosePopups) &&
		 !(g.LastItemData.InFlags & ImGuiItemFlags_SelectableDontClosePopup))
	{
		CloseCurrentPopup();
	}

	if(disabled_item && !disabled_global)
	{
		EndDisabled();
	}

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed; //-V1020
}

IGFD_API void IGFD::KeyExplorerFeature::prStartFlashItem(size_t vIdx)
{
	prFlashAlpha = 1.0f;
	prFlashedItem= vIdx;
}

IGFD_API bool IGFD::KeyExplorerFeature::prBeginFlashItem(size_t vIdx)
{
	bool res= false;

	if(prFlashedItem == vIdx && std::abs(prFlashAlpha - 0.0f) > 0.00001f)
	{
		prFlashAlpha-= prFlashAlphaAttenInSecs * ImGui::GetIO().DeltaTime;
		if(prFlashAlpha < 0.0f)
		{
			prFlashAlpha= 0.0f;
		}

		ImVec4 hov= ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered);
		hov.w     = prFlashAlpha;
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, hov);
		res= true;
	}

	return res;
}

IGFD_API void IGFD::KeyExplorerFeature::prEndFlashItem() { ImGui::PopStyleColor(); }

IGFD_API void IGFD::KeyExplorerFeature::SetFlashingAttenuationInSeconds(float vAttenValue)
{
	prFlashAlphaAttenInSecs= 1.0f / ImMax(vAttenValue, 0.01f);
}
	#endif // USE_EXPLORATION_BY_KEYS

	#pragma endregion

	#pragma endregion

	#pragma region FileDialog

IGFD_API IGFD::FileDialog::FileDialog()= default;

IGFD_API IGFD::FileDialog::~FileDialog()= default;

//////////////////////////////////////////////////////////////////////////////////////////////////
///// FILE DIALOG STANDARD DIALOG ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

// path and fileNameExt can be specified
IGFD_API void IGFD::FileDialog::OpenDialog(
		const std::string&   vKey,
		const std::string&   vTitle,
		const char*          vFilters,
		const std::string&   vPath,
		const std::string&   vFileName,
		const int&           vCountSelectionMax,
		UserDatas            vUserDatas,
		ImGuiFileDialogFlags vFlags
)
{
	if(prFileDialogInternal.puShowDialog) // if already opened, quit
	{
		return;
	}

	prFileDialogInternal.ResetForNewDialog();

	prFileDialogInternal.puDLGkey             = vKey;
	prFileDialogInternal.puDLGtitle           = vTitle;
	prFileDialogInternal.puDLGuserDatas       = vUserDatas;
	prFileDialogInternal.puDLGflags           = vFlags;
	prFileDialogInternal.puDLGoptionsPane     = nullptr;
	prFileDialogInternal.puDLGoptionsPaneWidth= 0.0f;

	prFileDialogInternal.puFilterManager.puDLGdefaultExt.clear();
	prFileDialogInternal.puFilterManager.ParseFilters(vFilters);

	prFileDialogInternal.puFileManager.puDLGDirectoryMode= (vFilters == nullptr);
	if(vPath.empty())
	{
		prFileDialogInternal.puFileManager.puDLGpath=
				prFileDialogInternal.puFileManager.GetCurrentPath();
	}
	else
	{
		prFileDialogInternal.puFileManager.puDLGpath= vPath;
	}
	prFileDialogInternal.puFileManager.SetCurrentPath(vPath);
	prFileDialogInternal.puFileManager.puDLGcountSelectionMax= (size_t)vCountSelectionMax;
	prFileDialogInternal.puFileManager.SetDefaultFileName(vFileName);

	prFileDialogInternal.puFileManager.ClearAll();

	prFileDialogInternal.puShowDialog= true; // open dialog
}

// path and filename are obtained from filePathName
IGFD_API void IGFD::FileDialog::OpenDialog(
		const std::string&   vKey,
		const std::string&   vTitle,
		const char*          vFilters,
		const std::string&   vFilePathName,
		const int&           vCountSelectionMax,
		UserDatas            vUserDatas,
		ImGuiFileDialogFlags vFlags
)
{
	if(prFileDialogInternal.puShowDialog) // if already opened, quit
	{
		return;
	}

	prFileDialogInternal.ResetForNewDialog();

	prFileDialogInternal.puDLGkey             = vKey;
	prFileDialogInternal.puDLGtitle           = vTitle;
	prFileDialogInternal.puDLGoptionsPane     = nullptr;
	prFileDialogInternal.puDLGoptionsPaneWidth= 0.0f;
	prFileDialogInternal.puDLGuserDatas       = vUserDatas;
	prFileDialogInternal.puDLGflags           = vFlags;

	auto ps= IGFD::Utils::ParsePathFileName(vFilePathName);
	if(ps.isOk)
	{
		prFileDialogInternal.puFileManager.puDLGpath= ps.path;
		prFileDialogInternal.puFileManager.SetDefaultFileName(ps.name);
		prFileDialogInternal.puFilterManager.puDLGdefaultExt= "." + ps.ext;
	}
	else
	{
		prFileDialogInternal.puFileManager.puDLGpath=
				prFileDialogInternal.puFileManager.GetCurrentPath();
		prFileDialogInternal.puFileManager.SetDefaultFileName("");
		prFileDialogInternal.puFilterManager.puDLGdefaultExt.clear();
	}

	prFileDialogInternal.puFilterManager.ParseFilters(vFilters);
	prFileDialogInternal.puFilterManager.SetSelectedFilterWithExt(
			prFileDialogInternal.puFilterManager.puDLGdefaultExt
	);

	prFileDialogInternal.puFileManager.SetCurrentPath(prFileDialogInternal.puFileManager.puDLGpath);

	prFileDialogInternal.puFileManager.puDLGDirectoryMode    = (vFilters == nullptr);
	prFileDialogInternal.puFileManager.puDLGcountSelectionMax= vCountSelectionMax; //-V101

	prFileDialogInternal.puFileManager.ClearAll();

	prFileDialogInternal.puShowDialog= true;
}

// with pane
// path and fileNameExt can be specified
IGFD_API void IGFD::FileDialog::OpenDialogWithPane(
		const std::string&   vKey,
		const std::string&   vTitle,
		const char*          vFilters,
		const std::string&   vPath,
		const std::string&   vFileName,
		const PaneFun&       vSidePane,
		const float&         vSidePaneWidth,
		const int&           vCountSelectionMax,
		UserDatas            vUserDatas,
		ImGuiFileDialogFlags vFlags
)
{
	if(prFileDialogInternal.puShowDialog) // if already opened, quit
	{
		return;
	}

	prFileDialogInternal.ResetForNewDialog();

	prFileDialogInternal.puDLGkey      = vKey;
	prFileDialogInternal.puDLGtitle    = vTitle;
	prFileDialogInternal.puDLGuserDatas= vUserDatas;
	prFileDialogInternal.puDLGflags    = vFlags;

	prFileDialogInternal.puDLGoptionsPane= vSidePane;
	if(prFileDialogInternal.puDLGoptionsPane != nullptr)
	{
		prFileDialogInternal.puDLGoptionsPaneWidth= vSidePaneWidth;
	}
	else
	{
		prFileDialogInternal.puDLGoptionsPaneWidth= 0.0f;
	}

	prFileDialogInternal.puFilterManager.puDLGdefaultExt.clear();
	prFileDialogInternal.puFilterManager.ParseFilters(vFilters);

	prFileDialogInternal.puFileManager.puDLGcountSelectionMax= (size_t)vCountSelectionMax;
	prFileDialogInternal.puFileManager.puDLGDirectoryMode    = (vFilters == nullptr);
	if(vPath.empty())
	{
		prFileDialogInternal.puFileManager.puDLGpath=
				prFileDialogInternal.puFileManager.GetCurrentPath();
	}
	else
	{
		prFileDialogInternal.puFileManager.puDLGpath= vPath;
	}

	prFileDialogInternal.puFileManager.SetCurrentPath(prFileDialogInternal.puFileManager.puDLGpath);

	prFileDialogInternal.puFileManager.SetDefaultFileName(vFileName);

	prFileDialogInternal.puFileManager.ClearAll();

	prFileDialogInternal.puShowDialog= true; // open dialog
}

// with pane
// path and filename are obtained from filePathName
IGFD_API void IGFD::FileDialog::OpenDialogWithPane(
		const std::string&   vKey,
		const std::string&   vTitle,
		const char*          vFilters,
		const std::string&   vFilePathName,
		const PaneFun&       vSidePane,
		const float&         vSidePaneWidth,
		const int&           vCountSelectionMax,
		UserDatas            vUserDatas,
		ImGuiFileDialogFlags vFlags
)
{
	if(prFileDialogInternal.puShowDialog) // if already opened, quit
	{
		return;
	}

	prFileDialogInternal.ResetForNewDialog();

	prFileDialogInternal.puDLGkey  = vKey;
	prFileDialogInternal.puDLGtitle= vTitle;

	prFileDialogInternal.puDLGoptionsPane= vSidePane;
	if(prFileDialogInternal.puDLGoptionsPane != nullptr)
	{
		prFileDialogInternal.puDLGoptionsPaneWidth= vSidePaneWidth;
	}
	else
	{
		prFileDialogInternal.puDLGoptionsPaneWidth= 0.0f;
	}
	prFileDialogInternal.puDLGuserDatas= vUserDatas;
	prFileDialogInternal.puDLGflags    = vFlags;

	auto ps= IGFD::Utils::ParsePathFileName(vFilePathName);
	if(ps.isOk)
	{
		prFileDialogInternal.puFileManager.puDLGpath= ps.path;
		prFileDialogInternal.puFileManager.SetDefaultFileName(vFilePathName);
		prFileDialogInternal.puFilterManager.puDLGdefaultExt= "." + ps.ext;
	}
	else
	{
		prFileDialogInternal.puFileManager.puDLGpath=
				prFileDialogInternal.puFileManager.GetCurrentPath();
		prFileDialogInternal.puFileManager.SetDefaultFileName("");
		prFileDialogInternal.puFilterManager.puDLGdefaultExt.clear();
	}

	prFileDialogInternal.puFileManager.SetCurrentPath(prFileDialogInternal.puFileManager.puDLGpath);

	prFileDialogInternal.puFileManager.puDLGcountSelectionMax= vCountSelectionMax; //-V101
	prFileDialogInternal.puFileManager.puDLGDirectoryMode    = (vFilters == nullptr);
	prFileDialogInternal.puFilterManager.ParseFilters(vFilters);
	prFileDialogInternal.puFilterManager.SetSelectedFilterWithExt(
			prFileDialogInternal.puFilterManager.puDLGdefaultExt
	);

	prFileDialogInternal.puFileManager.ClearAll();

	prFileDialogInternal.puShowDialog= true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
///// FILE DIALOG DISPLAY FUNCTION ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

IGFD_API bool IGFD::FileDialog::Display(
		const std::string& vKey,
		ImGuiWindowFlags   vFlags,
		ImVec2             vMinSize,
		ImVec2             vMaxSize
)
{
	bool res= false;

	if(prFileDialogInternal.puShowDialog && prFileDialogInternal.puDLGkey == vKey)
	{
		if(prFileDialogInternal.puUseCustomLocale)
		{
			setlocale(prFileDialogInternal.puLocaleCategory, prFileDialogInternal.puLocaleBegin.c_str());
		}

		auto& fdFile  = prFileDialogInternal.puFileManager;
		auto& fdFilter= prFileDialogInternal.puFilterManager;

		static ImGuiWindowFlags flags; // todo: not a good solution for multi instance, to fix

		// to be sure than only one dialog is displayed per frame
		ImGuiContext& g= *GImGui;
		if(g.FrameCount ==
			 prFileDialogInternal.puLastImGuiFrameCount) // one instance was displayed this frame before
																									 // for this key +> quit
		{
			return res;
		}
		prFileDialogInternal.puLastImGuiFrameCount=
				g.FrameCount; // mark this instance as used this frame

		std::string name= prFileDialogInternal.puDLGtitle + "##" + prFileDialogInternal.puDLGkey;
		if(prFileDialogInternal.puName != name)
		{
			fdFile.ClearComposer();
			fdFile.ClearFileLists();
			flags= vFlags;
		}

		NewFrame();

	#ifdef IMGUI_HAS_VIEWPORT
		if(!ImGui::GetIO().ConfigViewportsNoDecoration)
		{
			// https://github.com/ocornut/imgui/issues/4534
			ImGuiWindowClass window_class;
			window_class.ViewportFlagsOverrideClear= ImGuiViewportFlags_NoDecoration;
			ImGui::SetNextWindowClass(&window_class);
		}
	#endif // IMGUI_HAS_VIEWPORT

		bool beg= false;
		if(prFileDialogInternal.puDLGflags &
			 ImGuiFileDialogFlags_NoDialog) // disable our own dialog system (standard or modal)
		{
			beg= true;
		}
		else
		{
			ImGui::SetNextWindowSizeConstraints(vMinSize, vMaxSize);

			if(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_Modal &&
				 !prFileDialogInternal.puOkResultToConfirm) // disable modal because the confirm dialog for
																										// overwrite is a new modal
			{
				ImGui::OpenPopup(name.c_str());
				beg= ImGui::BeginPopupModal(
						name.c_str(), (bool*)nullptr, flags | ImGuiWindowFlags_NoScrollbar
				);
			}
			else
			{
				beg= ImGui::Begin(name.c_str(), (bool*)nullptr, flags | ImGuiWindowFlags_NoScrollbar);
			}
		}
		if(beg)
		{
	#ifdef IMGUI_HAS_VIEWPORT
			// if decoration is enabled we disable the resizing feature of imgui for avoid crash with SDL2
			// and GLFW3
			if(ImGui::GetIO().ConfigViewportsNoDecoration)
			{
				flags= vFlags;
			}
			else
			{
				auto win= ImGui::GetCurrentWindowRead();
				if(win->Viewport->Idx != 0)
				{
					flags|= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
				}
				else
				{
					flags= vFlags;
				}
			}
	#endif // IMGUI_HAS_VIEWPORT

			ImGuiID _frameId = ImGui::GetID(name.c_str());
			ImVec2  frameSize= ImVec2(0, 0);
			if(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_NoDialog)
			{
				frameSize= vMaxSize;
			}
			if(ImGui::BeginChild(_frameId, frameSize, false, flags | ImGuiWindowFlags_NoScrollbar))
			{
				prFileDialogInternal.puName= name; //-V820

				if(fdFile.puDLGpath.empty())
				{
					fdFile.puDLGpath= "."; // defaut path is '.'
				}

				fdFilter.SetDefaultFilterIfNotDefined();

				// init list of files
				if(fdFile.IsFileListEmpty() && !fdFile.puShowDrives)
				{
					IGFD::Utils::ReplaceString(
							fdFile.puDLGDefaultFileName, fdFile.puDLGpath,
							""
					); // local path
					if(!fdFile.puDLGDefaultFileName.empty())
					{
						fdFile.SetDefaultFileName(fdFile.puDLGDefaultFileName);
						fdFilter.SetSelectedFilterWithExt(fdFilter.puDLGdefaultExt);
					}
					else if(fdFile.puDLGDirectoryMode) // directory mode
					{
						fdFile.SetDefaultFileName(".");
					}
					fdFile.ScanDir(prFileDialogInternal, fdFile.puDLGpath);
				}

				// draw dialog parts
				prDrawHeader();      // bookmark, directory, path
				prDrawContent();     // bookmark, files view, side pane
				res= prDrawFooter(); // file field, filter combobox, ok/cancel buttons

				EndFrame();
			}
			ImGui::EndChild();

			// for display in dialog center, the confirm to overwrite dlg
			prFileDialogInternal.puDialogCenterPos=
					ImGui::GetCurrentWindowRead()->ContentRegionRect.GetCenter();

			// when the confirm to overwrite dialog will appear we need to
			// disable the modal mode of the main file dialog
			// see prOkResultToConfirm under
			if(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_Modal &&
				 !prFileDialogInternal.puOkResultToConfirm)
			{
				ImGui::EndPopup();
			}
		}

		if(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_NoDialog)
		{ // disable our own dialog system (standard or modal)
		}
		else
		{
			// same things here regarding prOkResultToConfirm
			if(!(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_Modal) ||
				 prFileDialogInternal.puOkResultToConfirm)
			{
				ImGui::End();
			}
		}
		// confirm the result and show the confirm to overwrite dialog if needed
		res= prConfirm_Or_OpenOverWriteFileDialog_IfNeeded(res, vFlags);

		if(prFileDialogInternal.puUseCustomLocale)
		{
			setlocale(prFileDialogInternal.puLocaleCategory, prFileDialogInternal.puLocaleEnd.c_str());
		}
	}

	return res;
}

IGFD_API void IGFD::FileDialog::NewFrame()
{
	prFileDialogInternal.NewFrame();
	NewThumbnailFrame(prFileDialogInternal);
}

IGFD_API void IGFD::FileDialog::EndFrame()
{
	EndThumbnailFrame(prFileDialogInternal);
	prFileDialogInternal.EndFrame();
}

IGFD_API void IGFD::FileDialog::QuitFrame() { QuitThumbnailFrame(prFileDialogInternal); }

IGFD_API void IGFD::FileDialog::prDrawHeader()
{
	#ifdef USE_BOOKMARK
	if(!(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableBookmarkMode))
	{
		prDrawBookmarkButton();
		ImGui::SameLine();
	}

	#endif // USE_BOOKMARK

	prFileDialogInternal.puFileManager.DrawDirectoryCreation(prFileDialogInternal);

	if(
	#ifdef USE_BOOKMARK
			!(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableBookmarkMode) ||
	#endif // USE_BOOKMARK
			!(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableCreateDirectoryButton))
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
	}
	prFileDialogInternal.puFileManager.DrawPathComposer(prFileDialogInternal);

	#ifdef USE_THUMBNAILS
	if(!(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableThumbnailMode))
	{
		prDrawDisplayModeToolBar();
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
	}
	#endif // USE_THUMBNAILS

	prFileDialogInternal.puSearchManager.DrawSearchBar(prFileDialogInternal);
}

IGFD_API void IGFD::FileDialog::prDrawContent()
{
	ImVec2 size= ImGui::GetContentRegionAvail();
	size.y-= prFileDialogInternal.puFooterHeight;

	#ifdef USE_BOOKMARK
	if(!(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableBookmarkMode))
	{
		if(prBookmarkPaneShown)
		{
			// size.x -= prBookmarkWidth;
			float otherWidth= size.x - prBookmarkWidth;
			ImGui::PushID("##splitterbookmark");
			IGFD::Utils::ImSplitter(
					true, 4.0f, &prBookmarkWidth, &otherWidth, 10.0f,
					10.0f + prFileDialogInternal.puDLGoptionsPaneWidth, size.y
			);
			ImGui::PopID();
			size.x-= otherWidth;
			prDrawBookmarkPane(prFileDialogInternal, size);
			ImGui::SameLine();
		}
	}
	#endif // USE_BOOKMARK

	size.x= ImGui::GetContentRegionAvail().x - prFileDialogInternal.puDLGoptionsPaneWidth;

	if(prFileDialogInternal.puDLGoptionsPane)
	{
		ImGui::PushID("##splittersidepane");
		IGFD::Utils::ImSplitter(
				true, 4.0f, &size.x, &prFileDialogInternal.puDLGoptionsPaneWidth, 10.0f, 10.0f, size.y
		);
		ImGui::PopID();
	}

	#ifdef USE_THUMBNAILS
	if(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableThumbnailMode)
	{
		prDrawFileListView(size);
	}
	else
	{
		switch(prDisplayMode)
		{
			case DisplayModeEnum::FILE_LIST: prDrawFileListView(size); break;
			case DisplayModeEnum::THUMBNAILS_LIST: prDrawThumbnailsListView(size); break;
			case DisplayModeEnum::THUMBNAILS_GRID: prDrawThumbnailsGridView(size); break;
		}
	}
	#else  // USE_THUMBNAILS
	prDrawFileListView(size);
	#endif // USE_THUMBNAILS

	if(prFileDialogInternal.puDLGoptionsPane)
	{
		prDrawSidePane(size.y);
	}

	if(!(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableQuickPathSelection))
	{
		DisplayPathPopup(size);
	}
}

IGFD_API void IGFD::FileDialog::DisplayPathPopup(ImVec2 vSize)
{
	ImVec2 size= ImVec2(vSize.x * 0.5f, vSize.y * 0.5f);
	if(ImGui::BeginPopup("IGFD_Path_Popup"))
	{
		auto& fdi= prFileDialogInternal.puFileManager;

		ImGui::PushID(this);

		static ImGuiTableFlags flags= ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
																	ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY |
																	ImGuiTableFlags_NoHostExtendY;
		auto listViewID= ImGui::GetID("##FileDialog_pathTable");
		if(ImGui::BeginTableEx("##FileDialog_pathTable", listViewID, 1, flags, size, 0.0f)) //-V112
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
			ImGui::TableSetupColumn(
					tableHeaderFileNameString,
					ImGuiTableColumnFlags_WidthStretch |
							(defaultSortOrderFilename ? ImGuiTableColumnFlags_PreferSortAscending
																				: ImGuiTableColumnFlags_PreferSortDescending),
					-1, 0
			);

			ImGui::TableHeadersRow();

			if(!fdi.IsPathFilteredListEmpty())
			{
				std::string _str;
				ImFont*     _font     = nullptr;
				bool        _showColor= false;

				prPathListClipper.Begin(
						(int)fdi.GetPathFilteredListSize(), ImGui::GetTextLineHeightWithSpacing()
				);
				while(prPathListClipper.Step())
				{
					for(int i= prPathListClipper.DisplayStart; i < prPathListClipper.DisplayEnd; i++)
					{
						if(i < 0)
						{
							continue;
						}

						auto infos= fdi.GetFilteredPathAt((size_t)i);
						if(!infos.use_count())
						{
							continue;
						}

						prBeginFileColorIconStyle(infos, _showColor, _str, &_font);

						bool selected= fdi.IsFileNameSelected(infos->fileNameExt); // found

						ImGui::TableNextRow();

						if(ImGui::TableNextColumn()) // file name
						{
							constexpr int32 flag1= static_cast<int32>(ImGuiSelectableFlags_SpanAllColumns);
							constexpr int32 flag2= static_cast<int32>(ImGuiSelectableFlags_SpanAvailWidth);
							if(ImGui::Selectable(infos->fileNameExt.c_str(), &selected, flag1 | flag2))
							{
								fdi.SetCurrentPath(fdi.ComposeNewPath(fdi.GetCurrentPopupComposedPath()));
								fdi.puPathClicked= fdi.SelectDirectory(infos);
								ImGui::CloseCurrentPopup();
							}
						}

						prEndFileColorIconStyle(_showColor, _font);
					}
				}
				prPathListClipper.End();
			}

			ImGui::EndTable();
		}

		ImGui::PopID();

		ImGui::EndPopup();
	}
}

IGFD_API bool IGFD::FileDialog::prDrawOkButton()
{
	auto& fdFile= prFileDialogInternal.puFileManager;
	if(prFileDialogInternal.puCanWeContinue && strlen(fdFile.puFileNameBuffer.data()))
	{
		if(IMGUI_BUTTON(okButtonString "##validationdialog", ImVec2(okButtonWidth, 0.0f)) ||
			 prFileDialogInternal.puIsOk)
		{
			prFileDialogInternal.puIsOk= true;
			return true;
		}

	#if !invertOkAndCancelButtons
		ImGui::SameLine();
	#endif
	}

	return false;
}

IGFD_API bool IGFD::FileDialog::prDrawCancelButton()
{
	if(IMGUI_BUTTON(cancelButtonString "##validationdialog", ImVec2(cancelButtonWidth, 0.0f)) ||
		 prFileDialogInternal.puNeedToExitDialog) // dialog exit asked
	{
		prFileDialogInternal.puIsOk= false;
		return true;
	}

	#if invertOkAndCancelButtons
	ImGui::SameLine();
	#endif

	return false;
}

IGFD_API bool IGFD::FileDialog::prDrawValidationButtons()
{
	bool res= false;

	ImGui::SetCursorPosX(
			ImGui::GetCursorPosX() +
			(ImGui::GetContentRegionAvail().x - prOkCancelButtonWidth) * okCancelButtonAlignement
	);

	ImGui::BeginGroup();

	if(invertOkAndCancelButtons)
	{
		res|= prDrawCancelButton();
		res|= prDrawOkButton();
	}
	else
	{
		res|= prDrawOkButton();
		res|= prDrawCancelButton();
	}

	ImGui::EndGroup();

	prOkCancelButtonWidth= ImGui::GetItemRectSize().x;

	return res;
}

IGFD_API bool IGFD::FileDialog::prDrawFooter()
{
	auto& fdFile= prFileDialogInternal.puFileManager;

	float posY= ImGui::GetCursorPos().y; // height of last bar calc
	ImGui::AlignTextToFramePadding();
	if(!fdFile.puDLGDirectoryMode)
	{
		ImGui::Text(fileNameString);
	}
	else // directory chooser
	{
		ImGui::Text(dirNameString);
	}
	ImGui::SameLine();

	// Input file fields
	float width= ImGui::GetContentRegionAvail().x;
	if(!fdFile.puDLGDirectoryMode)
	{
		ImGuiContext& g= *GImGui;
		width-= prFileDialogInternal.puFilterManager.GetFilterComboBoxWidth() + g.Style.ItemSpacing.x;
	}

	ImGui::PushItemWidth(width);
	ImGuiInputTextFlags flags= ImGuiInputTextFlags_EnterReturnsTrue;
	if(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_ReadOnlyFileNameField)
	{
		flags|= ImGuiInputTextFlags_ReadOnly;
	}
	if(ImGui::InputText(
				 "##FileName", fdFile.puFileNameBuffer.data(), MAX_FILE_DIALOG_NAME_BUFFER, flags
		 ))
	{
		prFileDialogInternal.puIsOk= true;
	}
	if(ImGui::GetItemID() == ImGui::GetActiveID())
	{
		prFileDialogInternal.puFileInputIsActive= true;
	}
	ImGui::PopItemWidth();

	// combobox of filters
	prFileDialogInternal.puFilterManager.DrawFilterComboBox(prFileDialogInternal);

	bool res                           = prDrawValidationButtons();
	prFileDialogInternal.puFooterHeight= ImGui::GetCursorPosY() - posY;
	return res;
}

IGFD_API void IGFD::FileDialog::prSelectableItem(
		int                        vidx,
		std::shared_ptr<FileInfos> vInfos,
		bool                       vSelected,
		const char*                vFmt,
		...
)
{
	if(!vInfos.use_count())
	{
		return;
	}

	auto& fdi= prFileDialogInternal.puFileManager;

	static ImGuiSelectableFlags selectableFlags= ImGuiSelectableFlags_AllowDoubleClick |
																							 ImGuiSelectableFlags_SpanAllColumns |
																							 ImGuiSelectableFlags_SpanAvailWidth;

	va_list args{};
	va_start(args, vFmt);
	vsnprintf(fdi.puVariadicBuffer.data(), MAX_FILE_DIALOG_NAME_BUFFER, vFmt, args);
	va_end(args);

	float h= 0.0f;
	#ifdef USE_THUMBNAILS
	if(prDisplayMode == DisplayModeEnum::THUMBNAILS_LIST &&
		 !(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_DisableThumbnailMode))
	{
		h= DisplayMode_ThumbailsList_ImageHeight;
	}
	#endif // USE_THUMBNAILS
	#ifdef USE_EXPLORATION_BY_KEYS
	bool flashed= prBeginFlashItem((size_t)vidx);
	bool res    = prFlashableSelectable(
      fdi.puVariadicBuffer, vSelected, selectableFlags, flashed, ImVec2(-1.0f, h)
  );
	if(flashed)
	{
		prEndFlashItem();
	}
	#else  // USE_EXPLORATION_BY_KEYS
	(void)vidx; // remove a warnings ofr unused var

	bool res=
			ImGui::Selectable(fdi.puVariadicBuffer.data(), vSelected, selectableFlags, ImVec2(-1.0f, h));
	#endif // USE_EXPLORATION_BY_KEYS
	if(res)
	{
		if(vInfos->fileType.isDir())
		{
			// nav system, selectable cause open directory or select directory
			if(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)
			{
				// little fix for get back the mouse behavior in nav system
				if(ImGui::IsMouseDoubleClicked(0)) // 0 -> left mouse button double click
				{
					fdi.puPathClicked= fdi.SelectDirectory(vInfos);
				}
				else if(fdi.puDLGDirectoryMode) // directory chooser
				{
					fdi.SelectFileName(prFileDialogInternal, vInfos);
				}
				else
				{
					fdi.puPathClicked= fdi.SelectDirectory(vInfos);
				}
			}
			else // no nav system => classic behavior
			{
				if(ImGui::IsMouseDoubleClicked(0)) // 0 -> left mouse button double click
				{
					fdi.puPathClicked= fdi.SelectDirectory(vInfos);
				}
				else if(fdi.puDLGDirectoryMode) // directory chooser
				{
					fdi.SelectFileName(prFileDialogInternal, vInfos);
				}
			}
		}
		else
		{
			fdi.SelectFileName(prFileDialogInternal, vInfos);

			if(ImGui::IsMouseDoubleClicked(0))
			{
				prFileDialogInternal.puIsOk= true;
			}
		}
	}
}

IGFD_API void IGFD::FileDialog::prBeginFileColorIconStyle(
		std::shared_ptr<FileInfos> vFileInfos,
		bool&                      vOutShowColor,
		std::string&               vOutStr,
		ImFont**                   vOutFont
)
{
	vOutStr.clear();
	vOutShowColor= false;

	if(vFileInfos->fileStyle.use_count()) //-V807 //-V522
	{
		vOutShowColor= true;

		*vOutFont= vFileInfos->fileStyle->font;
	}

	if(vOutShowColor && !vFileInfos->fileStyle->icon.empty())
	{
		vOutStr= vFileInfos->fileStyle->icon;
	}
	else if(vFileInfos->fileType.isDir())
	{
		vOutStr= dirEntryString;
	}
	else if(vFileInfos->fileType.isLinkToUnknown())
	{
		vOutStr= linkEntryString;
	}
	else if(vFileInfos->fileType.isFile())
	{
		vOutStr= fileEntryString;
	}

	vOutStr+= " " + vFileInfos->fileNameExt;

	if(vOutShowColor)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, vFileInfos->fileStyle->color);
	}
	if(*vOutFont != nullptr)
	{
		ImGui::PushFont(*vOutFont);
	}
}

IGFD_API void IGFD::FileDialog::prEndFileColorIconStyle(const bool& vShowColor, ImFont* vFont)
{
	if(vFont != nullptr)
	{
		ImGui::PopFont();
	}
	if(vShowColor)
	{
		ImGui::PopStyleColor();
	}
}

IGFD_API void IGFD::FileDialog::prDrawFileListView(ImVec2 vSize)
{
	auto& fdi= prFileDialogInternal.puFileManager;

	ImGui::PushID(this);

	static ImGuiTableFlags flags= ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
																ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY |
																ImGuiTableFlags_NoHostExtendY
	#ifndef USE_CUSTOM_SORTING_ICON
																| ImGuiTableFlags_Sortable
	#endif // USE_CUSTOM_SORTING_ICON
			;
	auto listViewID= ImGui::GetID("##FileDialog_fileTable");
	if(ImGui::BeginTableEx("##FileDialog_fileTable", listViewID, 4, flags, vSize, 0.0f)) //-V112
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(
				fdi.puHeaderFileName.c_str(),
				ImGuiTableColumnFlags_WidthStretch |
						(defaultSortOrderFilename ? ImGuiTableColumnFlags_PreferSortAscending
																			: ImGuiTableColumnFlags_PreferSortDescending),
				-1, 0
		);
		ImGui::TableSetupColumn(
				fdi.puHeaderFileType.c_str(),
				ImGuiTableColumnFlags_WidthFixed |
						(defaultSortOrderType ? ImGuiTableColumnFlags_PreferSortAscending
																	: ImGuiTableColumnFlags_PreferSortDescending) |
						((prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_HideColumnType)
								 ? ImGuiTableColumnFlags_DefaultHide
								 : 0),
				-1, 1
		);
		ImGui::TableSetupColumn(
				fdi.puHeaderFileSize.c_str(),
				ImGuiTableColumnFlags_WidthFixed |
						(defaultSortOrderSize ? ImGuiTableColumnFlags_PreferSortAscending
																	: ImGuiTableColumnFlags_PreferSortDescending) |
						((prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_HideColumnSize)
								 ? ImGuiTableColumnFlags_DefaultHide
								 : 0),
				-1, 2
		);
		ImGui::TableSetupColumn(
				fdi.puHeaderFileDate.c_str(),
				ImGuiTableColumnFlags_WidthFixed |
						(defaultSortOrderDate ? ImGuiTableColumnFlags_PreferSortAscending
																	: ImGuiTableColumnFlags_PreferSortDescending) |
						((prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_HideColumnDate)
								 ? ImGuiTableColumnFlags_DefaultHide
								 : 0),
				-1, 3
		);

	#ifndef USE_CUSTOM_SORTING_ICON
		// Sort our data if sort specs have been changed!
		if(ImGuiTableSortSpecs* sorts_specs= ImGui::TableGetSortSpecs())
		{
			if(sorts_specs->SpecsDirty && !fdi.IsFileListEmpty())
			{
				bool direction= sorts_specs->Specs->SortDirection == ImGuiSortDirection_Ascending;

				if(sorts_specs->Specs->ColumnUserID == 0)
				{
					fdi.puSortingField       = IGFD::FileManager::SortingFieldEnum::FIELD_FILENAME;
					fdi.puSortingDirection[0]= direction;
					fdi.SortFields(prFileDialogInternal);
				}
				else if(sorts_specs->Specs->ColumnUserID == 1)
				{
					fdi.puSortingField       = IGFD::FileManager::SortingFieldEnum::FIELD_TYPE;
					fdi.puSortingDirection[1]= direction;
					fdi.SortFields(prFileDialogInternal);
				}
				else if(sorts_specs->Specs->ColumnUserID == 2)
				{
					fdi.puSortingField       = IGFD::FileManager::SortingFieldEnum::FIELD_SIZE;
					fdi.puSortingDirection[2]= direction;
					fdi.SortFields(prFileDialogInternal);
				}
				else // if (sorts_specs->Specs->ColumnUserID == 3) => alwayd true for the moment, to
						 // uncomment if we add a fourth column
				{
					fdi.puSortingField       = IGFD::FileManager::SortingFieldEnum::FIELD_DATE;
					fdi.puSortingDirection[3]= direction;
					fdi.SortFields(prFileDialogInternal);
				}

				sorts_specs->SpecsDirty= false;
			}
		}

		ImGui::TableHeadersRow();
	#else  // USE_CUSTOM_SORTING_ICON
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		for(int column= 0; column < 4; column++) //-V112
		{
			ImGui::TableSetColumnIndex(column);
			const char* column_name=
					ImGui::TableGetColumnName(column); // Retrieve name passed to TableSetupColumn()
			ImGui::PushID(column);
			ImGui::TableHeader(column_name);
			ImGui::PopID();
			if(ImGui::IsItemClicked())
			{
				if(column == 0)
				{
					if(fdi.puSortingField == IGFD::FileManager::SortingFieldEnum::FIELD_FILENAME)
					{
						fdi.puSortingDirection[0]= !fdi.puSortingDirection[0];
					}
					else
					{
						fdi.puSortingField= IGFD::FileManager::SortingFieldEnum::FIELD_FILENAME;
					}

					fdi.SortFields(prFileDialogInternal);
				}
				else if(column == 1)
				{
					if(fdi.puSortingField == IGFD::FileManager::SortingFieldEnum::FIELD_TYPE)
					{
						fdi.puSortingDirection[1]= !fdi.puSortingDirection[1];
					}
					else
					{
						fdi.puSortingField= IGFD::FileManager::SortingFieldEnum::FIELD_TYPE;
					}

					fdi.SortFields(prFileDialogInternal);
				}
				else if(column == 2)
				{
					if(fdi.puSortingField == IGFD::FileManager::SortingFieldEnum::FIELD_SIZE)
					{
						fdi.puSortingDirection[2]= !fdi.puSortingDirection[2];
					}
					else
					{
						fdi.puSortingField= IGFD::FileManager::SortingFieldEnum::FIELD_SIZE;
					}

					fdi.SortFields(prFileDialogInternal);
				}
				else // if (column == 3) => alwayd true for the moment, to uncomment if we add a fourth
						 // column
				{
					if(fdi.puSortingField == IGFD::FileManager::SortingFieldEnum::FIELD_DATE)
					{
						fdi.puSortingDirection[3]= !fdi.puSortingDirection[3];
					}
					else
					{
						fdi.puSortingField= IGFD::FileManager::SortingFieldEnum::FIELD_DATE;
					}

					fdi.SortFields(prFileDialogInternal);
				}
			}
		}
	#endif // USE_CUSTOM_SORTING_ICON
		if(!fdi.IsFilteredListEmpty())
		{
			std::string _str;
			ImFont*     _font     = nullptr;
			bool        _showColor= false;

			prFileListClipper.Begin(
					(int)fdi.GetFilteredListSize(), ImGui::GetTextLineHeightWithSpacing()
			);
			while(prFileListClipper.Step())
			{
				for(int i= prFileListClipper.DisplayStart; i < prFileListClipper.DisplayEnd; i++)
				{
					if(i < 0)
					{
						continue;
					}

					auto infos= fdi.GetFilteredFileAt((size_t)i);
					if(!infos.use_count())
					{
						continue;
					}

					prBeginFileColorIconStyle(infos, _showColor, _str, &_font);

					bool selected= fdi.IsFileNameSelected(infos->fileNameExt); // found

					ImGui::TableNextRow();

					if(ImGui::TableNextColumn()) // file name
					{
						prSelectableItem(i, infos, selected, _str.c_str());
					}
					if(ImGui::TableNextColumn()) // file type
					{
						ImGui::Text("%s", infos->fileExtLevels[0].c_str());
					}
					if(ImGui::TableNextColumn()) // file size
					{
						if(!infos->fileType.isDir())
						{
							ImGui::Text("%s ", infos->formatedFileSize.c_str());
						}
						else
						{
							ImGui::TextUnformatted("");
						}
					}
					if(ImGui::TableNextColumn()) // file date + time
					{
						ImGui::Text("%s", infos->fileModifDate.c_str());
					}

					prEndFileColorIconStyle(_showColor, _font);
				}
			}
			prFileListClipper.End();
		}

	#ifdef USE_EXPLORATION_BY_KEYS
		if(!fdi.puInputPathActivated)
		{
			prLocateByInputKey(prFileDialogInternal);
			prExploreWithkeys(prFileDialogInternal, listViewID);
		}
	#endif // USE_EXPLORATION_BY_KEYS

		ImGuiContext& g= *GImGui;
		if(g.LastActiveId - 1 == listViewID || g.LastActiveId == listViewID)
		{
			prFileDialogInternal.puFileListViewIsActive= true;
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
}

	#ifdef USE_THUMBNAILS
IGFD_API void IGFD::FileDialog::prDrawThumbnailsListView(ImVec2 vSize)
{
	auto& fdi= prFileDialogInternal.puFileManager;

	ImGui::PushID(this);

	static ImGuiTableFlags flags= ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
																ImGuiTableFlags_Hideable | ImGuiTableFlags_ScrollY |
																ImGuiTableFlags_NoHostExtendY
		#ifndef USE_CUSTOM_SORTING_ICON
																| ImGuiTableFlags_Sortable
		#endif // USE_CUSTOM_SORTING_ICON
			;
	auto listViewID= ImGui::GetID("##FileDialog_fileTable");
	if(ImGui::BeginTableEx("##FileDialog_fileTable", listViewID, 5, flags, vSize, 0.0f))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(
				fdi.puHeaderFileName.c_str(),
				ImGuiTableColumnFlags_WidthStretch |
						(defaultSortOrderFilename ? ImGuiTableColumnFlags_PreferSortAscending
																			: ImGuiTableColumnFlags_PreferSortDescending),
				-1, 0
		);
		ImGui::TableSetupColumn(
				fdi.puHeaderFileType.c_str(),
				ImGuiTableColumnFlags_WidthFixed |
						(defaultSortOrderType ? ImGuiTableColumnFlags_PreferSortAscending
																	: ImGuiTableColumnFlags_PreferSortDescending) |
						((prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_HideColumnType)
								 ? ImGuiTableColumnFlags_DefaultHide
								 : 0),
				-1, 1
		);
		ImGui::TableSetupColumn(
				fdi.puHeaderFileSize.c_str(),
				ImGuiTableColumnFlags_WidthFixed |
						(defaultSortOrderSize ? ImGuiTableColumnFlags_PreferSortAscending
																	: ImGuiTableColumnFlags_PreferSortDescending) |
						((prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_HideColumnSize)
								 ? ImGuiTableColumnFlags_DefaultHide
								 : 0),
				-1, 2
		);
		ImGui::TableSetupColumn(
				fdi.puHeaderFileDate.c_str(),
				ImGuiTableColumnFlags_WidthFixed |
						(defaultSortOrderDate ? ImGuiTableColumnFlags_PreferSortAscending
																	: ImGuiTableColumnFlags_PreferSortDescending) |
						((prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_HideColumnDate)
								 ? ImGuiTableColumnFlags_DefaultHide
								 : 0),
				-1, 3
		);
		// not needed to have an option for hide the thumbnails since this is why this view is used
		ImGui::TableSetupColumn(
				fdi.puHeaderFileThumbnails.c_str(),
				ImGuiTableColumnFlags_WidthFixed |
						(defaultSortOrderThumbnails ? ImGuiTableColumnFlags_PreferSortAscending
																				: ImGuiTableColumnFlags_PreferSortDescending),
				-1, 4
		); //-V112

		#ifndef USE_CUSTOM_SORTING_ICON
		// Sort our data if sort specs have been changed!
		if(ImGuiTableSortSpecs* sorts_specs= ImGui::TableGetSortSpecs())
		{
			if(sorts_specs->SpecsDirty && !fdi.IsFileListEmpty())
			{
				bool direction= sorts_specs->Specs->SortDirection == ImGuiSortDirection_Ascending;

				if(sorts_specs->Specs->ColumnUserID == 0)
				{
					fdi.puSortingField       = IGFD::FileManager::SortingFieldEnum::FIELD_FILENAME;
					fdi.puSortingDirection[0]= direction;
					fdi.SortFields(prFileDialogInternal);
				}
				else if(sorts_specs->Specs->ColumnUserID == 1)
				{
					fdi.puSortingField       = IGFD::FileManager::SortingFieldEnum::FIELD_TYPE;
					fdi.puSortingDirection[1]= direction;
					fdi.SortFields(prFileDialogInternal);
				}
				else if(sorts_specs->Specs->ColumnUserID == 2)
				{
					fdi.puSortingField       = IGFD::FileManager::SortingFieldEnum::FIELD_SIZE;
					fdi.puSortingDirection[2]= direction;
					fdi.SortFields(prFileDialogInternal);
				}
				else if(sorts_specs->Specs->ColumnUserID == 3)
				{
					fdi.puSortingField       = IGFD::FileManager::SortingFieldEnum::FIELD_DATE;
					fdi.puSortingDirection[3]= direction;
					fdi.SortFields(prFileDialogInternal);
				}
				else // if (sorts_specs->Specs->ColumnUserID == 4) = > always true for the moment, to
						 // uncomment if we add another column
				{
					fdi.puSortingField       = IGFD::FileManager::SortingFieldEnum::FIELD_THUMBNAILS;
					fdi.puSortingDirection[4]= direction;
					fdi.SortFields(prFileDialogInternal);
				}

				sorts_specs->SpecsDirty= false;
			}
		}

		ImGui::TableHeadersRow();
		#else  // USE_CUSTOM_SORTING_ICON
		ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
		for(int column= 0; column < 5; column++)
		{
			ImGui::TableSetColumnIndex(column);
			const char* column_name=
					ImGui::TableGetColumnName(column); // Retrieve name passed to TableSetupColumn()
			ImGui::PushID(column);
			ImGui::TableHeader(column_name);
			ImGui::PopID();
			if(ImGui::IsItemClicked())
			{
				if(column == 0)
				{
					if(fdi.puSortingField == IGFD::FileManager::SortingFieldEnum::FIELD_FILENAME)
					{
						fdi.puSortingDirection[0]= !fdi.puSortingDirection[0];
					}
					else
					{
						fdi.puSortingField= IGFD::FileManager::SortingFieldEnum::FIELD_FILENAME;
					}

					fdi.SortFields(prFileDialogInternal);
				}
				else if(column == 1)
				{
					if(fdi.puSortingField == IGFD::FileManager::SortingFieldEnum::FIELD_TYPE)
					{
						fdi.puSortingDirection[1]= !fdi.puSortingDirection[1];
					}
					else
					{
						fdi.puSortingField= IGFD::FileManager::SortingFieldEnum::FIELD_TYPE;
					}

					fdi.SortFields(prFileDialogInternal);
				}
				else if(column == 2)
				{
					if(fdi.puSortingField == IGFD::FileManager::SortingFieldEnum::FIELD_SIZE)
					{
						fdi.puSortingDirection[2]= !fdi.puSortingDirection[2];
					}
					else
					{
						fdi.puSortingField= IGFD::FileManager::SortingFieldEnum::FIELD_SIZE;
					}

					fdi.SortFields(prFileDialogInternal);
				}
				else if(column == 3)
				{
					if(fdi.puSortingField == IGFD::FileManager::SortingFieldEnum::FIELD_DATE)
					{
						fdi.puSortingDirection[3]= !fdi.puSortingDirection[3];
					}
					else
					{
						fdi.puSortingField= IGFD::FileManager::SortingFieldEnum::FIELD_DATE;
					}

					fdi.SortFields(prFileDialogInternal);
				}
				else // if (sorts_specs->Specs->ColumnUserID == 4) = > always true for the moment, to
						 // uncomment if we add another column
				{
					if(fdi.puSortingField == IGFD::FileManager::SortingFieldEnum::FIELD_THUMBNAILS)
					{
						fdi.puSortingDirection[4]= !fdi.puSortingDirection[4];
					}
					else
					{
						fdi.puSortingField= IGFD::FileManager::SortingFieldEnum::FIELD_THUMBNAILS;
					}

					fdi.SortFields(prFileDialogInternal);
				}
			}
		}
		#endif // USE_CUSTOM_SORTING_ICON
		if(!fdi.IsFilteredListEmpty())
		{
			std::string _str;
			ImFont*     _font     = nullptr;
			bool        _showColor= false;

			ImGuiContext& g= *GImGui;
			const float   itemHeight=
					ImMax(g.FontSize, DisplayMode_ThumbailsList_ImageHeight) + g.Style.ItemSpacing.y;

			prFileListClipper.Begin((int)fdi.GetFilteredListSize(), itemHeight);
			while(prFileListClipper.Step())
			{
				for(int i= prFileListClipper.DisplayStart; i < prFileListClipper.DisplayEnd; i++)
				{
					if(i < 0)
					{
						continue;
					}

					auto infos= fdi.GetFilteredFileAt((size_t)i);
					if(!infos.use_count())
					{
						continue;
					}

					prBeginFileColorIconStyle(infos, _showColor, _str, &_font);

					bool selected= fdi.IsFileNameSelected(infos->fileNameExt); // found

					ImGui::TableNextRow();

					if(ImGui::TableNextColumn()) // file name
					{
						prSelectableItem(i, infos, selected, _str.c_str());
					}
					if(ImGui::TableNextColumn()) // file type
					{
						ImGui::Text("%s", infos->fileExtLevels[0].c_str());
					}
					if(ImGui::TableNextColumn()) // file size
					{
						if(!infos->fileType.isDir())
						{
							ImGui::Text("%s ", infos->formatedFileSize.c_str());
						}
						else
						{
							ImGui::TextUnformatted("");
						}
					}
					if(ImGui::TableNextColumn()) // file date + time
					{
						ImGui::Text("%s", infos->fileModifDate.c_str());
					}
					if(ImGui::TableNextColumn()) // file thumbnails
					{
						auto th= &infos->thumbnailInfo;

						if(!th->isLoadingOrLoaded)
						{
							prAddThumbnailToLoad(infos);
						}
						if(th->isReadyToDisplay && th->textureID)
						{
							ImGui::Image(
									(ImTextureID)th->textureID,
									ImVec2((float)th->textureWidth, (float)th->textureHeight)
							);
						}
					}

					prEndFileColorIconStyle(_showColor, _font);
				}
			}
			prFileListClipper.End();
		}

		#ifdef USE_EXPLORATION_BY_KEYS
		if(!fdi.puInputPathActivated)
		{
			prLocateByInputKey(prFileDialogInternal);
			prExploreWithkeys(prFileDialogInternal, listViewID);
		}
		#endif // USE_EXPLORATION_BY_KEYS

		ImGuiContext& g= *GImGui;
		if(g.LastActiveId - 1 == listViewID || g.LastActiveId == listViewID)
		{
			prFileDialogInternal.puFileListViewIsActive= true;
		}

		ImGui::EndTable();
	}

	ImGui::PopID();
}

IGFD_API void IGFD::FileDialog::prDrawThumbnailsGridView(ImVec2 vSize)
{
	if(ImGui::BeginChild("##thumbnailsGridsFiles", vSize))
	{
		// todo
	}

	ImGui::EndChild();
}

	#endif

IGFD_API void IGFD::FileDialog::prDrawSidePane(float vHeight)
{
	ImGui::SameLine();

	ImGui::BeginChild("##FileTypes", ImVec2(0, vHeight));

	prFileDialogInternal.puDLGoptionsPane(
			prFileDialogInternal.puFilterManager.GetSelectedFilter().getFirstFilter().c_str(),
			prFileDialogInternal.puDLGuserDatas, &prFileDialogInternal.puCanWeContinue
	);
	ImGui::EndChild();
}

IGFD_API void IGFD::FileDialog::Close()
{
	prFileDialogInternal.puDLGkey.clear();
	prFileDialogInternal.puShowDialog= false;
}

IGFD_API bool IGFD::FileDialog::WasOpenedThisFrame(const std::string& vKey) const
{
	bool res= prFileDialogInternal.puShowDialog && prFileDialogInternal.puDLGkey == vKey;
	if(res)
	{
		res&= prFileDialogInternal.puLastImGuiFrameCount ==
					GImGui->FrameCount; // return true if a dialog was displayed in this frame
	}
	return res;
}

IGFD_API bool IGFD::FileDialog::WasOpenedThisFrame() const
{
	bool res= prFileDialogInternal.puShowDialog;
	if(res)
	{
		res&= prFileDialogInternal.puLastImGuiFrameCount ==
					GImGui->FrameCount; // return true if a dialog was displayed in this frame
	}
	return res;
}

IGFD_API bool IGFD::FileDialog::IsOpened(const std::string& vKey) const
{
	return (prFileDialogInternal.puShowDialog && prFileDialogInternal.puDLGkey == vKey);
}

IGFD_API bool IGFD::FileDialog::IsOpened() const { return prFileDialogInternal.puShowDialog; }

IGFD_API std::string IGFD::FileDialog::GetOpenedKey() const
{
	if(prFileDialogInternal.puShowDialog)
	{
		return prFileDialogInternal.puDLGkey;
	}
	return "";
}

IGFD_API std::string IGFD::FileDialog::GetFilePathName(IGFD_ResultMode vFlag)
{
	return prFileDialogInternal.puFileManager.GetResultingFilePathName(prFileDialogInternal, vFlag);
}

IGFD_API std::string IGFD::FileDialog::GetCurrentPath()
{
	return prFileDialogInternal.puFileManager.GetResultingPath();
}

IGFD_API std::string IGFD::FileDialog::GetCurrentFileName(IGFD_ResultMode vFlag)
{
	return prFileDialogInternal.puFileManager.GetResultingFileName(prFileDialogInternal, vFlag);
}

IGFD_API std::string IGFD::FileDialog::GetCurrentFilter()
{
	return prFileDialogInternal.puFilterManager.GetSelectedFilter().title;
}

IGFD_API std::map<std::string, std::string> IGFD::FileDialog::GetSelection(IGFD_ResultMode vFlag)
{
	return prFileDialogInternal.puFileManager.GetResultingSelection(prFileDialogInternal, vFlag);
}

IGFD_API IGFD::UserDatas IGFD::FileDialog::GetUserDatas() const
{
	return prFileDialogInternal.puDLGuserDatas;
}

IGFD_API bool IGFD::FileDialog::IsOk() const { return prFileDialogInternal.puIsOk; }

IGFD_API void IGFD::FileDialog::SetFileStyle(
		const IGFD_FileStyleFlags& vFlags,
		const char*                vCriteria,
		const FileStyle&           vInfos
)
{
	prFileDialogInternal.puFilterManager.SetFileStyle(vFlags, vCriteria, vInfos);
}

IGFD_API void IGFD::FileDialog::SetFileStyle(
		const IGFD_FileStyleFlags& vFlags,
		const char*                vCriteria,
		const ImVec4&              vColor,
		const std::string&         vIcon,
		ImFont*                    vFont
)
{
	prFileDialogInternal.puFilterManager.SetFileStyle(vFlags, vCriteria, vColor, vIcon, vFont);
}

IGFD_API void IGFD::FileDialog::SetFileStyle(FileStyle::FileStyleFunctor vFunctor)
{
	prFileDialogInternal.puFilterManager.SetFileStyle(vFunctor);
}

IGFD_API bool IGFD::FileDialog::GetFileStyle(
		const IGFD_FileStyleFlags& vFlags,
		const std::string&         vCriteria,
		ImVec4*                    vOutColor,
		std::string* const         vOutIcon,
		ImFont**                   vOutFont
)
{
	return prFileDialogInternal.puFilterManager.GetFileStyle(
			vFlags, vCriteria, vOutColor, vOutIcon, vOutFont
	);
}

IGFD_API void IGFD::FileDialog::ClearFilesStyle()
{
	prFileDialogInternal.puFilterManager.ClearFilesStyle();
}

IGFD_API void IGFD::FileDialog::SetLocales(
		const int& /*vLocaleCategory*/,
		const std::string& vLocaleBegin,
		const std::string& vLocaleEnd
)
{
	prFileDialogInternal.puUseCustomLocale= true;
	prFileDialogInternal.puLocaleBegin    = vLocaleBegin;
	prFileDialogInternal.puLocaleEnd      = vLocaleEnd;
}

//////////////////////////////////////////////////////////////////////////////
//// OVERWRITE DIALOG ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

IGFD_API bool IGFD::FileDialog::prConfirm_Or_OpenOverWriteFileDialog_IfNeeded(
		bool             vLastAction,
		ImGuiWindowFlags vFlags
)
{
	// if confirmation => return true for confirm the overwrite et quit the dialog
	// if cancel => return false && set IsOk to false for keep inside the dialog

	// if IsOk == false => return false for quit the dialog
	if(!prFileDialogInternal.puIsOk && vLastAction)
	{
		QuitFrame();
		return true;
	}

	// if IsOk == true && no check of overwrite => return true for confirm the dialog
	if(prFileDialogInternal.puIsOk && vLastAction &&
		 !(prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_ConfirmOverwrite))
	{
		QuitFrame();
		return true;
	}

	// if IsOk == true && check of overwrite => return false and show confirm to overwrite dialog
	if((prFileDialogInternal.puOkResultToConfirm || (prFileDialogInternal.puIsOk && vLastAction)) &&
		 (prFileDialogInternal.puDLGflags & ImGuiFileDialogFlags_ConfirmOverwrite))
	{
		if(prFileDialogInternal.puIsOk) // catched only one time
		{
			if(!prFileDialogInternal.puFileManager.IsFileExist(GetFilePathName()
				 )) // not existing => quit dialog
			{
				QuitFrame();
				return true;
			}
			else // existing => confirm dialog to open
			{
				prFileDialogInternal.puIsOk             = false;
				prFileDialogInternal.puOkResultToConfirm= true;
			}
		}

		std::string name= OverWriteDialogTitleString "##" + prFileDialogInternal.puDLGtitle +
											prFileDialogInternal.puDLGkey + "OverWriteDialog";

		bool res= false;

		ImGui::OpenPopup(name.c_str());
		if(ImGui::BeginPopupModal(
					 name.c_str(), (bool*)0,
					 vFlags | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize |
							 ImGuiWindowFlags_NoMove
			 ))
		{
			ImVec2 windowPos= prFileDialogInternal.puDialogCenterPos;
			windowPos.x-= ImGui::GetWindowSize().x * 0.5f;
			windowPos.y-= ImGui::GetWindowSize().y * 0.5f;
			ImGui::SetWindowPos(windowPos); // next frame needed for GetWindowSize to work

			ImGui::Text("%s", OverWriteDialogMessageString);

			if(IMGUI_BUTTON(OverWriteDialogConfirmButtonString))
			{
				prFileDialogInternal.puOkResultToConfirm= false;
				prFileDialogInternal.puIsOk             = true;
				res                                     = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if(IMGUI_BUTTON(OverWriteDialogCancelButtonString))
			{
				prFileDialogInternal.puOkResultToConfirm= false;
				prFileDialogInternal.puIsOk             = false;
				res                                     = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if(res)
		{
			QuitFrame();
		}
		return res;
	}

	return false;
}

	#pragma endregion

	#pragma endregion

#endif // __cplusplus

#pragma region IGFD_C_API

// Return an initialized IGFD_Selection_Pair
IGFD_C_API IGFD_Selection_Pair IGFD_Selection_Pair_Get(void)
{
	IGFD_Selection_Pair res= {};
	res.fileName           = nullptr;
	res.filePathName       = nullptr;
	return res;
}

// destroy only the content of vSelection_Pair
IGFD_C_API void IGFD_Selection_Pair_DestroyContent(IGFD_Selection_Pair* vSelection_Pair)
{
	if(vSelection_Pair != nullptr)
	{
		delete[] vSelection_Pair->fileName;
		delete[] vSelection_Pair->filePathName;
	}
}

// Return an initialized IGFD_Selection
IGFD_C_API IGFD_Selection IGFD_Selection_Get(void) { return { nullptr, 0U }; }

// destroy only the content of vSelection
IGFD_C_API void IGFD_Selection_DestroyContent(IGFD_Selection* vSelection)
{
	if(vSelection != nullptr)
	{
		if(vSelection->table != nullptr)
		{
			for(size_t i= 0U; i < vSelection->count; i++)
			{
				IGFD_Selection_Pair_DestroyContent(&vSelection->table[i]);
			}
			delete[] vSelection->table;
		}
		vSelection->count= 0U;
	}
}

// create an instance of ImGuiFileDialog
IGFD_C_API ImGuiFileDialog* IGFD_Create(void) { return new ImGuiFileDialog(); }

// destroy the instance of ImGuiFileDialog
IGFD_C_API void IGFD_Destroy(ImGuiFileDialog* vContextPtr)
{
	if(vContextPtr != nullptr)
	{
		delete vContextPtr;
		vContextPtr= nullptr;
	}
}

// standard dialog
IGFD_C_API void IGFD_OpenDialog(
		ImGuiFileDialog*     vContextPtr,
		const char*          vKey,
		const char*          vTitle,
		const char*          vFilters,
		const char*          vPath,
		const char*          vFileName,
		const int            vCountSelectionMax,
		void*                vUserDatas,
		ImGuiFileDialogFlags flags
)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->OpenDialog(
				vKey, vTitle, vFilters, vPath, vFileName, vCountSelectionMax, vUserDatas, flags
		);
	}
}

IGFD_C_API void IGFD_OpenDialog2(
		ImGuiFileDialog*     vContextPtr,
		const char*          vKey,
		const char*          vTitle,
		const char*          vFilters,
		const char*          vFilePathName,
		const int            vCountSelectionMax,
		void*                vUserDatas,
		ImGuiFileDialogFlags flags
)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->OpenDialog(
				vKey, vTitle, vFilters, vFilePathName, vCountSelectionMax, vUserDatas, flags
		);
	}
}

IGFD_C_API void IGFD_OpenDialogWithPane(
		ImGuiFileDialog*     vContextPtr,
		const char*          vKey,
		const char*          vTitle,
		const char*          vFilters,
		const char*          vPath,
		const char*          vFileName,
		IGFD_PaneFun         vSidePane,
		const float          vSidePaneWidth,
		const int            vCountSelectionMax,
		void*                vUserDatas,
		ImGuiFileDialogFlags flags
)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->OpenDialogWithPane(
				vKey, vTitle, vFilters, vPath, vFileName, vSidePane, vSidePaneWidth, vCountSelectionMax,
				vUserDatas, flags
		);
	}
}

IGFD_C_API void IGFD_OpenDialogWithPane2(
		ImGuiFileDialog*     vContextPtr,
		const char*          vKey,
		const char*          vTitle,
		const char*          vFilters,
		const char*          vFilePathName,
		IGFD_PaneFun         vSidePane,
		const float          vSidePaneWidth,
		const int            vCountSelectionMax,
		void*                vUserDatas,
		ImGuiFileDialogFlags flags
)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->OpenDialogWithPane(
				vKey, vTitle, vFilters, vFilePathName, vSidePane, vSidePaneWidth, vCountSelectionMax,
				vUserDatas, flags
		);
	}
}

IGFD_C_API bool IGFD_DisplayDialog(
		ImGuiFileDialog* vContextPtr,
		const char*      vKey,
		ImGuiWindowFlags vFlags,
		ImVec2           vMinSize,
		ImVec2           vMaxSize
)
{
	if(vContextPtr != nullptr)
	{
		return vContextPtr->Display(vKey, vFlags, vMinSize, vMaxSize);
	}

	return false;
}

IGFD_C_API void IGFD_CloseDialog(ImGuiFileDialog* vContextPtr)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->Close();
	}
}

IGFD_C_API bool IGFD_IsOk(ImGuiFileDialog* vContextPtr)
{
	if(vContextPtr != nullptr)
	{
		return vContextPtr->IsOk();
	}

	return false;
}

IGFD_C_API bool IGFD_WasKeyOpenedThisFrame(ImGuiFileDialog* vContextPtr, const char* vKey)
{
	if(vContextPtr != nullptr)
	{
		return vContextPtr->WasOpenedThisFrame(vKey);
	}

	return false;
}

IGFD_C_API bool IGFD_WasOpenedThisFrame(ImGuiFileDialog* vContextPtr)
{
	if(vContextPtr != nullptr)
	{
		return vContextPtr->WasOpenedThisFrame();
	}

	return false;
}

IGFD_C_API bool IGFD_IsKeyOpened(ImGuiFileDialog* vContextPtr, const char* vCurrentOpenedKey)
{
	if(vContextPtr != nullptr)
	{
		return vContextPtr->IsOpened(vCurrentOpenedKey);
	}

	return false;
}

IGFD_C_API bool IGFD_IsOpened(ImGuiFileDialog* vContextPtr)
{
	if(vContextPtr != nullptr)
	{
		return vContextPtr->IsOpened();
	}

	return false;
}

IGFD_C_API IGFD_Selection IGFD_GetSelection(ImGuiFileDialog* vContextPtr, IGFD_ResultMode vMode)
{
	IGFD_Selection res= IGFD_Selection_Get();

	if(vContextPtr != nullptr)
	{
		auto sel= vContextPtr->GetSelection(vMode);
		if(!sel.empty())
		{
			res.count= sel.size();
			res.table= new IGFD_Selection_Pair[res.count];

			size_t idx= 0U;
			for(const auto& s : sel)
			{
				IGFD_Selection_Pair* pair= res.table + idx++;

				// fileNameExt
				if(!s.first.empty())
				{
					size_t siz    = s.first.size() + 1U;
					pair->fileName= new char[siz];
#ifndef _MSC_VER
					strncpy(pair->fileName, s.first.c_str(), siz);
#else  // _MSC_VER
					strncpy_s(pair->fileName, siz, s.first.c_str(), siz);
#endif // _MSC_VER
					pair->fileName[siz - 1U]= '\0';
				}

				// filePathName
				if(!s.second.empty())
				{
					size_t siz        = s.second.size() + 1U;
					pair->filePathName= new char[siz];
#ifndef _MSC_VER
					strncpy(pair->filePathName, s.second.c_str(), siz);
#else  // _MSC_VER
					strncpy_s(pair->filePathName, siz, s.second.c_str(), siz);
#endif // _MSC_VER
					pair->filePathName[siz - 1U]= '\0';
				}
			}

			return res;
		}
	}

	return res;
}

IGFD_C_API char* IGFD_GetFilePathName(ImGuiFileDialog* vContextPtr, IGFD_ResultMode vMode)
{
	char* res= nullptr;

	if(vContextPtr != nullptr)
	{
		auto s= vContextPtr->GetFilePathName(vMode);
		if(!s.empty())
		{
			size_t siz= s.size() + 1U;
			res       = (char*)malloc(siz);
			if(res != nullptr)
			{
#ifndef _MSC_VER
				strncpy(res, s.c_str(), siz);
#else  // _MSC_VER
				strncpy_s(res, siz, s.c_str(), siz);
#endif // _MSC_VER
				res[siz - 1U]= '\0';
			}
		}
	}

	return res;
}

IGFD_C_API char* IGFD_GetCurrentFileName(ImGuiFileDialog* vContextPtr, IGFD_ResultMode vMode)
{
	char* res= nullptr;

	if(vContextPtr != nullptr)
	{
		auto s= vContextPtr->GetCurrentFileName(vMode);
		if(!s.empty())
		{
			size_t siz= s.size() + 1U;
			res       = (char*)malloc(siz);
			if(res != nullptr)
			{
#ifndef _MSC_VER
				strncpy(res, s.c_str(), siz);
#else  // _MSC_VER
				strncpy_s(res, siz, s.c_str(), siz);
#endif // _MSC_VER
				res[siz - 1U]= '\0';
			}
		}
	}

	return res;
}

IGFD_C_API char* IGFD_GetCurrentPath(ImGuiFileDialog* vContextPtr)
{
	char* res= nullptr;

	if(vContextPtr != nullptr)
	{
		auto s= vContextPtr->GetCurrentPath();
		if(!s.empty())
		{
			size_t siz= s.size() + 1U;
			res       = (char*)malloc(siz);
			if(res != nullptr)
			{
#ifndef _MSC_VER
				strncpy(res, s.c_str(), siz);
#else  // _MSC_VER
				strncpy_s(res, siz, s.c_str(), siz);
#endif // _MSC_VER
				res[siz - 1U]= '\0';
			}
		}
	}

	return res;
}

IGFD_C_API char* IGFD_GetCurrentFilter(ImGuiFileDialog* vContextPtr)
{
	char* res= nullptr;

	if(vContextPtr != nullptr)
	{
		auto s= vContextPtr->GetCurrentFilter();
		if(!s.empty())
		{
			size_t siz= s.size() + 1U;
			res       = (char*)malloc(siz);
			if(res != nullptr)
			{
#ifndef _MSC_VER
				strncpy(res, s.c_str(), siz);
#else  // _MSC_VER
				strncpy_s(res, siz, s.c_str(), siz);
#endif // _MSC_VER
				res[siz - 1U]= '\0';
			}
		}
	}

	return res;
}

IGFD_C_API void* IGFD_GetUserDatas(ImGuiFileDialog* vContextPtr)
{
	if(vContextPtr != nullptr)
	{
		return vContextPtr->GetUserDatas();
	}

	return nullptr;
}

IGFD_C_API void IGFD_SetFileStyle(
		ImGuiFileDialog*    vContextPtr,
		IGFD_FileStyleFlags vFlags,
		const char*         vCriteria,
		ImVec4              vColor,
		const char*         vIcon,
		ImFont*             vFont
) //-V813
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->SetFileStyle(vFlags, vCriteria, vColor, vIcon, vFont);
	}
}

IGFD_C_API void IGFD_SetFileStyle2(
		ImGuiFileDialog*    vContextPtr,
		IGFD_FileStyleFlags vFlags,
		const char*         vCriteria,
		float               vR,
		float               vG,
		float               vB,
		float               vA,
		const char*         vIcon,
		ImFont*             vFont
)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->SetFileStyle(vFlags, vCriteria, ImVec4(vR, vG, vB, vA), vIcon, vFont);
	}
}

IGFD_C_API bool IGFD_GetFileStyle(
		ImGuiFileDialog*    vContextPtr,
		IGFD_FileStyleFlags vFlags,
		const char*         vCriteria,
		ImVec4*             vOutColor,
		char**              vOutIconText,
		ImFont**            vOutFont
)
{
	if(vContextPtr != nullptr)
	{
		std::string icon;
		bool        res= vContextPtr->GetFileStyle(vFlags, vCriteria, vOutColor, &icon, vOutFont);
		if(!icon.empty() && vOutIconText)
		{
			size_t siz   = icon.size() + 1U;
			*vOutIconText= (char*)malloc(siz);
			if(*vOutIconText != nullptr)
			{
#ifndef _MSC_VER
				strncpy(*vOutIconText, icon.c_str(), siz);
#else  // _MSC_VER
				strncpy_s(*vOutIconText, siz, icon.c_str(), siz);
#endif // _MSC_VER
				(*vOutIconText)[siz - 1U]= '\0';
			}
		}
		return res;
	}

	return false;
}

IGFD_C_API void IGFD_ClearFilesStyle(ImGuiFileDialog* vContextPtr)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->ClearFilesStyle();
	}
}

IGFD_C_API void SetLocales(
		ImGuiFileDialog* vContextPtr,
		const int        vCategory,
		const char*      vBeginLocale,
		const char*      vEndLocale
)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->SetLocales(
				vCategory, (vBeginLocale ? vBeginLocale : ""), (vEndLocale ? vEndLocale : "")
		);
	}
}

#ifdef USE_EXPLORATION_BY_KEYS
IGFD_C_API void
IGFD_SetFlashingAttenuationInSeconds(ImGuiFileDialog* vContextPtr, float vAttenValue)
{
	if(vContextPtr)
	{
		vContextPtr->SetFlashingAttenuationInSeconds(vAttenValue);
	}
}
#endif

#ifdef USE_BOOKMARK
IGFD_C_API char*
IGFD_SerializeBookmarks(ImGuiFileDialog* vContextPtr, bool vDontSerializeCodeBasedBookmarks)
{
	char* res= nullptr;

	if(vContextPtr)
	{
		auto s= vContextPtr->SerializeBookmarks(vDontSerializeCodeBasedBookmarks);
		if(!s.empty())
		{
			size_t siz= s.size() + 1U;
			res       = (char*)malloc(siz);
			if(res)
			{
	#ifndef _MSC_VER
				strncpy(res, s.c_str(), siz);
	#else  // _MSC_VER
				strncpy_s(res, siz, s.c_str(), siz);
	#endif // _MSC_VER
				res[siz - 1U]= '\0';
			}
		}
	}

	return res;
}

IGFD_C_API void IGFD_DeserializeBookmarks(ImGuiFileDialog* vContextPtr, const char* vBookmarks)
{
	if(vContextPtr)
	{
		vContextPtr->DeserializeBookmarks(vBookmarks);
	}
}

IGFD_C_API void
IGFD_AddBookmark(ImGuiFileDialog* vContextPtr, const char* vBookMarkName, const char* vBookMarkPath)
{
	if(vContextPtr)
	{
		vContextPtr->AddBookmark(vBookMarkName, vBookMarkPath);
	}
}

IGFD_C_API void IGFD_RemoveBookmark(ImGuiFileDialog* vContextPtr, const char* vBookMarkName)
{
	if(vContextPtr)
	{
		vContextPtr->RemoveBookmark(vBookMarkName);
	}
}

#endif

#ifdef USE_THUMBNAILS
IGFD_C_API void SetCreateThumbnailCallback(
		ImGuiFileDialog*              vContextPtr,
		const IGFD_CreateThumbnailFun vCreateThumbnailFun
)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->SetCreateThumbnailCallback(vCreateThumbnailFun);
	}
}

IGFD_C_API void SetDestroyThumbnailCallback(
		ImGuiFileDialog*               vContextPtr,
		const IGFD_DestroyThumbnailFun vDestroyThumbnailFun
)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->SetDestroyThumbnailCallback(vDestroyThumbnailFun);
	}
}

IGFD_C_API void ManageGPUThumbnails(ImGuiFileDialog* vContextPtr)
{
	if(vContextPtr != nullptr)
	{
		vContextPtr->ManageGPUThumbnails();
	}
}
#endif // USE_THUMBNAILS

#pragma endregion