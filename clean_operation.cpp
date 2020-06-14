#include <windows.h>
#include <winbase.h>

using namespace std;
#include "clean_operation.h"

const int space_limitation = 2;

bool return_module_path(String * const module_path,
	const String * const process) {
	if (!module_path || !process)
		return false;
	String exe_file;
	PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
	HANDLE h_p = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	bool find_process_okey = Process32First(h_p, &pe32);
	while (find_process_okey) {
		exe_file = String(pe32.szExeFile);
		if (exe_file.Pos("\\ ") != 0)
			exe_file = ExtractFileName(pe32.szExeFile);
		if (exe_file.Trim().UpperCase() == process->Trim().UpperCase()) {
			HANDLE h_m = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
				pe32.th32ProcessID);
			MODULEENTRY32 me = {sizeof(MODULEENTRY32)};
			BOOL find_module_okey = Module32First(h_m, &me);
			while (find_module_okey) {
				if (pe32.th32ProcessID == me.th32ProcessID) {
					break;
				}
				find_module_okey = Module32Next(h_m, &me);
			}
			CloseHandle(h_m);
			module_path->operator += (ExtractFilePath(me.szExePath));
			break;
		}
		find_process_okey = Process32Next(h_p, &pe32);
	}
	CloseHandle(h_p);
	return true;
}

bool return_rec_path(String * const rec_path, const String * const module_path)
{
	if (!rec_path || !module_path)
		return false;
	rec_path->operator = ("");
	String ini_file = *module_path + "WSServer.ini";
	if (FileExists(ini_file)) {
		TStringList *sl = new TStringList();
		sl->LoadFromFile(ini_file);
		unsigned line_count = sl->Count;
		String temp_line;
		String temp_str1;
		if ((temp_str1 = (temp_line = sl->Strings[68]).SubString(1, 12))
			== "Record Path=") {
			temp_str1 = temp_line.SubString(13, temp_line.Length());
			rec_path->operator += (temp_str1);
		}
		else {
			for (int i = 0; i < line_count; i++) {
				temp_line = sl->Strings[i];
				if ((temp_str1 = temp_line.SubString(1, 12)) == "Record Path=")
				{
					temp_str1 = temp_line.SubString(13, temp_line.Length());
					rec_path->operator += (temp_str1);
				}
			}
		}
		delete sl;
	}
	else {
		rec_path->operator += (*module_path + "Record");
	}
	return true;
}

bool get_disk_available_space(unsigned __int64 * const space,
	const String * const disk) {
	if (!disk || !space)
		return false;
	ULARGE_INTEGER free_bytes;
	String temp_disk = *disk;
	if (!::GetDiskFreeSpaceEx(temp_disk.t_str(), &free_bytes, NULL, NULL))
		return false;
	*space = free_bytes.QuadPart;
	return true;
}

bool delete_dir_n_kakyuusei(String * const dir) {
	if (!dir)
		return false;
	if (!DirectoryExists(*dir) && !FileExists(*dir))
		return false;
	String temp_dir = *dir;
	wchar_t buf[256];
	int i;
	for (i = 0; i < 256; i++) {
		buf[i] = '\0';
	}
	wchar_t *buf2 = temp_dir.t_str();
	for (i = 0; i < temp_dir.Length(); i++) {
		buf[i] = buf2[i];
	}
	SHFILEOPSTRUCT op_strc;
	op_strc.hwnd = NULL;
	op_strc.wFunc = FO_DELETE;
	op_strc.pFrom = buf;
	op_strc.pTo = NULL;
	op_strc.fFlags = FOF_NOCONFIRMATION;
	op_strc.fAnyOperationsAborted = FALSE;
	op_strc.hNameMappings = NULL;
	op_strc.lpszProgressTitle = NULL;
	SHFileOperation(&op_strc);
	return true;
}

bool has_last_backslash(bool * const has_last_backslash,
	const String * const str) {
	if (!str || !has_last_backslash)
		return false;
	if ("\\" == str->SubString(str->Length(), 1)) {
		*has_last_backslash = true;
	}
	else {
		*has_last_backslash = false;
	}
	return true;
}

bool find_dirs(TStringList * const goal_dirs, const String * const parent_dir,
	String * const substr) {
	if (!goal_dirs || !parent_dir)
		return false;
	bool r = true;
	WIN32_FIND_DATA fnd_data;
	String temp_parent;
	if ("\\" == parent_dir->SubString(parent_dir->Length(), 1)) {
		temp_parent = *parent_dir + "*";
	}
	else {
		temp_parent = *parent_dir + "\\*";
	}
	HANDLE h_fnd = INVALID_HANDLE_VALUE;
	h_fnd = FindFirstFile(temp_parent.t_str(), &fnd_data);
	if (INVALID_HANDLE_VALUE == h_fnd) {
		return r = false;
	}
	String temp_dir;
	do {
		if (FILE_ATTRIBUTE_DIRECTORY & fnd_data.dwFileAttributes) {
			temp_dir = fnd_data.cFileName;
			if (temp_dir == "." || temp_dir == "..") {
				continue;
			}
			if (!substr) {
				goal_dirs->Append(temp_dir);
			}
			else {
				if (temp_dir.Pos(*substr)) {
					goal_dirs->Append(temp_dir);
				}
			}
		}
	}
	while (0 != FindNextFile(h_fnd, &fnd_data));
	FindClose(h_fnd);
	return r;
}

bool get_date_from_dir(TDate * const date, const String * const dir) {
	if (!date || !dir)
		return false;
	bool r = true;
	String temp_dir = dir->Trim();
	String year, month, day;
	do {
		r = ((year = temp_dir.SubString(1, 4)) != "") &&
			(be_numberic_string(&year));
		if (!r)
			break;
		r = (((month = temp_dir.SubString(6, 2)) != "")) &&
			(be_numberic_string(&month));
		if (!r)
			break;
		if (((day = temp_dir.SubString(9, 2)) != "")) {
			if (!(r = be_numberic_string(&day)))
				break;
		}
		else {
			day = "1";
		}
		*date = EncodeDate(StrToInt(year), StrToInt(month), StrToInt(day));
	}
	while (false);
	return r;
}

bool month_to_string(String * const string, const TDate * const month,
	const bool need_leading_zero) {
	if (!string)
		return false;
	string->operator = ("");
	TDate current_month;
	if (!month) {
		current_month = Now();
	}
	else {
		current_month.operator = (*month);
	}
	string->operator = (FormatDateTime("yyyy-mm", current_month));
	return true;
}

bool return_earliest_month_dir(String * const earliest_dir,
	const String * const parent_dir) {
	if (!earliest_dir || !parent_dir)
		return false;
	bool r = true;
	int i, j, earliest_index;
	String one_dir;
	TDate earliest_month, temp_month;
	TStringList *months = new TStringList();
	earliest_month = Now();
	earliest_index = -1;
	do {
		r = find_dirs(months, parent_dir, NULL);
		if (!r)
			break;
		j = months->Count;
		for (i = 0; i < j; i++) {
			one_dir = months->Strings[i];
			if (!get_date_from_dir(&temp_month, &one_dir))
				continue;
			if (temp_month < earliest_month) {
				earliest_month = temp_month;
				earliest_index = i;
			}
		}
		if (-1 == earliest_index) {
			r = false;
		}
	}
	while (false);
	month_to_string(earliest_dir, &earliest_month, true);
	delete months;
	return r;
}

bool return_earliest_date_dir(String * const earliest_dir,
	const String * const month) {
	if (!earliest_dir || !month)
		return false;
	earliest_dir->operator = ("");
	bool r = true;
	int i, j, earliest_index;
	String one_dir;
	TDate earliest_date, temp_date;
	TStringList *days = new TStringList();
	earliest_date = Now();
	earliest_index = -1;
	bool r_bl;
	has_last_backslash(&r_bl, month);
	String temp_parent = *month;
	String temp_path = "";
	if (!r_bl) {
		temp_parent += "\\";
	}
	do {
		r = find_dirs(days, month, NULL);
		if (!r)
			break;
		j = days->Count;
		for (i = 0; i < j; i++) {
			one_dir = days->Strings[i];
			if (!get_date_from_dir(&temp_date, &one_dir))
				continue;
			if (temp_date < earliest_date) {
				earliest_date = temp_date;
				earliest_index = i;
			}
		}
		if (-1 == earliest_index) {
			r = false;
		}
	}
	while (false);
	date_to_string(earliest_dir, &earliest_date, true);
	delete days;
	return r;
}

bool date_to_string(String * const string, const TDate * const date,
	const bool need_leading_zero) {
	if (!string)
		return false;
	string->operator = ("");
	TDate current_date;
	if (!date) {
		current_date = Now();
	}
	else {
		current_date.operator = (*date);
	}
	string->operator = (FormatDateTime("yyyy-mm-dd", current_date));
	return true;
}

bool get_single_earliest_dir(String * const str,
	const String * const parent_dir) {
	if (!str || !parent_dir)
		return false;
	bool r = true;
	bool has_last_backslash1;
	String earliest_month, current_month, earliest_day, temp_parent_dir = "";
	temp_parent_dir.operator += (*parent_dir);
	has_last_backslash(&has_last_backslash1, parent_dir);
	if (!has_last_backslash1) {
		temp_parent_dir += "\\";
	}
	do {
		r = return_earliest_month_dir(&earliest_month, parent_dir);
		if (!r)
			break;
		String sub_str = temp_parent_dir + earliest_month;
		return_earliest_date_dir(&earliest_day, &sub_str);
		str->operator =
			(temp_parent_dir + earliest_month + "\\" + earliest_day);
	}
	while (false);
	return r;
}

bool get_files_to_delete(TStringList * const files, String * const rec_path) {
	if (!files || !rec_path)
		return false;
	bool r = true;
	TStringList *dates_to_delete = new TStringList;
	do {
		if (!dates_to_delete) {
			return r = false;
		}
		r = get_folds_to_delete(dates_to_delete, rec_path);
		if (!r)
			break;
		r = get_earliest_files(files, dates_to_delete);
	}
	while (false);
	delete dates_to_delete;
	return r;
}

bool get_earliest_files(TStringList * const files, TStringList * const dirs) {
	if (!files || !dirs)
		return false;
	bool r = true;
	files->Clear();
	int dir_num = dirs->Count;
	String one_dir;
	TStringList *temp_sl = new TStringList;
	if (!temp_sl)
		return r = false;
	int file_num;
	for (int i = 0; i < dir_num; i++) {
		one_dir = dirs->Strings[i] + "\\" + "VIDEO";
		temp_sl->Clear();
		file_num = get_files_in_fold(temp_sl, &one_dir);
		if (-1 == file_num) {
			continue;
		}
		else if (!file_num) {
		}
		else {
			temp_sl->Sort();
			files->Append(temp_sl->Strings[0]);
		}
	}
	delete temp_sl;
	return r;
}

int get_files_in_fold(TStringList * const files, String * const dir) {
	int result = 0;
	bool has_last_backslash1;
	String temp_dir = *dir;
	String temp_dir1 = *dir;
	if (!files || !dir)
		return result = -1;
	if (files->Count)
		files->Clear();
	has_last_backslash(&has_last_backslash1, dir);
	if (!has_last_backslash1) {
		temp_dir += "\\*";
		temp_dir1 += "\\";
	}
	else {
		temp_dir += "*";
	}
	WIN32_FIND_DATA fnd_data;
	HANDLE h_fnd = INVALID_HANDLE_VALUE;
	h_fnd = FindFirstFile(temp_dir.t_str(), &fnd_data);
	if (INVALID_HANDLE_VALUE == h_fnd) {
		return result = -1;
	}
	do {
		if (!(FILE_ATTRIBUTE_DIRECTORY & fnd_data.dwFileAttributes)) {
			files->Append(temp_dir1 + fnd_data.cFileName);
			result++;
		}
	}
	while (0 != FindNextFile(h_fnd, &fnd_data));
	FindClose(h_fnd);
	return result;
}

bool get_current_month_str(String * const month) {
	if (!month)
		return false;
	if (!month_to_string(month, NULL, true)) {
		month->operator = ("");
	}
	return true;
}

bool get_folds_to_delete(TStringList * const folds_to_delete,
	String * const rec_path) {
	if (!folds_to_delete || !rec_path)
		return false;
	bool r = true;
	bool has_last_backslash1;
	has_last_backslash(&has_last_backslash1, rec_path);
	String temp_rec_path = "";
	temp_rec_path.operator += (*rec_path);
	if (!has_last_backslash1) {
		temp_rec_path += "\\";
	}
	String temp_sub_rec_path = "";
	int i, j;
	String earliest_dir;
	TStringList *sub_rec_path = new TStringList;
	do {
		r = find_dirs(sub_rec_path, rec_path, NULL);
		if (!r)
			break;
		r = (j = sub_rec_path->Count);
		if (!r)
			break;
		for (i = 0; i < j; i++) {
			temp_sub_rec_path.operator = ("");
			temp_sub_rec_path = temp_rec_path + sub_rec_path->Strings[i];
			if (!get_single_earliest_dir(&earliest_dir, &temp_sub_rec_path))
				continue;
			folds_to_delete->Append(earliest_dir);
		}
	}
	while (false);
	delete sub_rec_path;
	return r;
}

bool be_numberic_string(const String * const string) {
	if (!string)
		return false;
	int j = string->Length() + 1;
	for (int i = 1; i < j; i++) {
		if (string->operator[](i) < '0' || string->operator[](i) > '9')
			return false;
	}
	return true;
}

bool exec_delete(const String * const process) {
	if (!process)
		return false;
	bool r = true;
	String driver = "";
	String log = "";
	unsigned __int64 availabel_space = 0;
	double available_G = 0;
	String module_path, rec_path;
	TStringList *files_to_delete = new TStringList;
	String debug_string;
	do {
		r = return_module_path(&module_path, process);
		if (!r)
			break;
		r = return_rec_path(&rec_path, &module_path);
		delete_empty_folds(&rec_path);
		if (!r)
			break;
		driver = ExtractFileDrive(module_path);
		while (get_disk_available_space(&availabel_space,
			&driver) && ((available_G =
			(double)((double)availabel_space / (double)1073741824)) <
			(double)space_limitation)) {
			debug_string = available_G;
			OutputDebugString(debug_string.t_str());
			r = get_files_to_delete(files_to_delete, &rec_path);
			if (!r)
				break;
			kickoff_today_files_n_folds(files_to_delete);
			for (int i = 0; i < files_to_delete->Count; i++) {
				log.operator = ("Jessica Alba: " + files_to_delete->Strings[i]);
				OutputDebugString(log.t_str());
				delete_dir_n_kakyuusei(&(files_to_delete->Strings[i]));

			}
		}
		delete_empty_folds(&rec_path);
	}
	while (false);
	delete files_to_delete;
	return r;
}

int find_empty_folds1(TStringList * const empty_folds,
	const String * const rec_path) {
	int err_num = -1;
	if (!empty_folds || !rec_path)
		return err_num;
	TStringList *dirs_level_scene = new TStringList;
	String temp_rec_path;
	temp_rec_path.operator = (*rec_path);
	bool hlb;
	has_last_backslash(&hlb, rec_path);
	if (!hlb) {
		temp_rec_path.operator += ("\\");
	}
	do {
		if (!dirs_level_scene)
			return err_num;
		err_num = find_dirs(dirs_level_scene, rec_path, NULL);
		if (!err_num) {
			err_num = -1;
			break;
		}
		empty_folds->Clear();
		TStringList *pathes_level_scene = new TStringList;
		for (int i = 0; i < dirs_level_scene->Count; i++) {
			pathes_level_scene->Strings[i].operator =
				(temp_rec_path + dirs_level_scene->Strings[i]);
		}
		if (!find_empty_folds_in_scenes(empty_folds, pathes_level_scene)) {
			err_num = -1;
			delete pathes_level_scene;
			break;
		}
		delete pathes_level_scene;
		err_num = 0;
	}
	while (false);
	delete dirs_level_scene;
	return err_num;
}

bool find_empty_folds_in_scenes(TStringList * const folds,
	TStringList * const scenes) {
	if (!folds || !scenes)
		return false;
	bool result = true;
	return result;
}

bool find_empty_folds(TStringList * const sub_level_folds, String * const fold)
{
	if (!sub_level_folds || !fold)
		return false;
	bool be_correct_process = true;
	int is_empty;
	WIN32_FIND_DATA fnd_data;
	String temp_fold;
	if ("\\" == fold->SubString(fold->Length(), 1)) {
		temp_fold = *fold + "*";
	}
	else {
		temp_fold = *fold + "\\*";
	}
	HANDLE h_fnd = INVALID_HANDLE_VALUE;
	h_fnd = FindFirstFile(temp_fold.t_str(), &fnd_data);
	if (INVALID_HANDLE_VALUE == h_fnd) {
		return be_correct_process = false;
	}
	String node;
	String full_path;
	do {
		is_empty = is_empty_fold(fold);
		if (-1 == is_empty)
			continue;
		if (0 == is_empty) {
			sub_level_folds->Append(*fold);
			break;
		}
		else {
			node = fnd_data.cFileName;
			if (FILE_ATTRIBUTE_DIRECTORY & fnd_data.dwFileAttributes) {
				if (node == "." || node == "..") {
					continue;
				}
				full_path.operator = (fold->operator +("\\" + node));
				find_empty_folds(sub_level_folds, &full_path);
			}
		}
	}
	while (0 != FindNextFile(h_fnd, &fnd_data));
	FindClose(h_fnd);
	return be_correct_process;
}

int is_empty_fold(String * const fold) {
	int err_num = -1;
	bool has_last_backslash1;
	HANDLE h_fnd = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fnd_data;
	String temp_fold = *fold;
	String node;
	if (!fold)
		return err_num;
	has_last_backslash(&has_last_backslash1, fold);
	if (has_last_backslash1) {
		temp_fold += "*";
	}
	else {
		temp_fold += "\\*";
	}
	h_fnd = FindFirstFile(temp_fold.t_str(), &fnd_data);
	if (INVALID_HANDLE_VALUE == h_fnd) {
		return err_num;
	}
	err_num++;
	do {
		if (FILE_ATTRIBUTE_DIRECTORY & fnd_data.dwFileAttributes) {
			node.operator = (fnd_data.cFileName);
			if (node.operator == (".") || node.operator == ("..")) {
				continue;
			}
			err_num++;
		}
		else {
			err_num++;
		}
	}
	while (0 != FindNextFile(h_fnd, &fnd_data));
	FindClose(h_fnd);
	return err_num;
}

bool find_empty_folds2(TStringList * const sub_empty_folds, String * const fold)
{
	if (!sub_empty_folds || !fold)
		return false;
	bool be_correct_process = true;
	WIN32_FIND_DATA fnd_data;
	String temp_fold;
	if ("\\" == fold->SubString(fold->Length(), 1)) {
		temp_fold.operator = (*fold + "*");
	}
	else {
		temp_fold.operator = (*fold + "\\*");
	}
	HANDLE h_fnd = INVALID_HANDLE_VALUE;
	h_fnd = FindFirstFile(temp_fold.t_str(), &fnd_data);
	if (INVALID_HANDLE_VALUE == h_fnd) {
		return be_correct_process = false;
	}
	String node;
	String full_path;
	int count = 0;
	do {
		node.operator = (fnd_data.cFileName);
		if (FILE_ATTRIBUTE_DIRECTORY & fnd_data.dwFileAttributes) {
			if (node.operator == (".") || node.operator == ("..")) {
				continue;
			}
			full_path.operator = (*fold + node);
			find_empty_folds(sub_empty_folds, &full_path);
			count++;
		}
		else {
			count++;
		}
	}
	while (0 != FindNextFile(h_fnd, &fnd_data));
	if (!count)
		sub_empty_folds->Append(*fold);
	FindClose(h_fnd);
	return be_correct_process;
}

bool delete_empty_folds(String * const path) {
	bool r = true;
	if (!path)
		return false;
	String log;
	TStringList *empty_folds = new TStringList;
	if (!find_empty_folds2(empty_folds, path))
		return r = false;
	kickoff_today_files_n_folds(empty_folds);
	for (int i = 0; i < empty_folds->Count; i++) {
		log.operator = ("Ozawa Maria: " + empty_folds->Strings[i]);
		OutputDebugString(log.t_str());
		delete_dir_n_kakyuusei(&(empty_folds->Strings[i]));
	}
	delete empty_folds;
	return r;
}

bool kickoff_today_files_n_folds(TStringList * const files_n_folds_to_delete) {
	bool corr_proc = true;
	if (!files_n_folds_to_delete)
		return false;
	TDate current_date = Now();
	String current_date_string;
	current_date_string.operator = (FormatDateTime("yyyy-mm-dd", current_date));
	int count = files_n_folds_to_delete->Count;
	String temp_str;
	for (int i = 0; i < count;) {
		temp_str = files_n_folds_to_delete->Strings[i];
		if (temp_str.Pos(current_date_string)) {
			files_n_folds_to_delete->Delete(i);
			count = files_n_folds_to_delete->Count;
		}
		else {
			i++;
		}
	}
	return corr_proc;
}
