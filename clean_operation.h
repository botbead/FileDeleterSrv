#ifndef CLEAN_OPERATION_H
#define CLEAN_OPERATION_H

#include <windows.h>
#include <Tlhelp32.h>
#include <vcl.h>
#include <Filectrl.hpp>

bool return_module_path(String * const , const String * const);
bool return_rec_path(String * const , const String * const);
bool get_disk_available_space(unsigned __int64 * const , const String * const);
bool delete_dir_n_kakyuusei(String * const);
bool get_folds_to_delete(TStringList * const , String * const);
bool has_last_backslash(bool * const , const String * const);
bool find_dirs(TStringList * const , const String * const , String * const);
bool get_single_earliest_dir(String * const , const String * const);
bool get_date_from_dir(TDate * const , const String * const);
bool get_files_to_delete(TStringList * const , String * const);
bool get_earliest_files(TStringList * const , TStringList *cons);
int get_files_in_fold(TStringList * const , String * const);
bool be_numberic_string(const String * const);
bool date_to_string(String * const , const TDate * const , const bool);
bool get_current_month_str(String * const);
bool month_to_string(String * const , const TDate * const , const bool);
bool return_earliest_month_dir(String * const , const String * const);
bool return_earliest_date_dir(String * const , const String * const);
bool exec_delete(const String * const);
int is_empty_fold(String * const);
int find_empty_folds1(TStringList * const , const String * const);
bool find_empty_folds_in_scenes(TStringList * const , TStringList * const);
bool find_empty_folds(TStringList * const , String * const);
bool find_empty_folds2(TStringList * const , String * const);
bool delete_empty_folds(String * const);
bool kickoff_today_files_n_folds(TStringList * const);

#endif
