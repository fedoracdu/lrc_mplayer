/*
 * =====================================================================================
 *
 *       Filename:  bluray.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  09/08/2011 08:14:50 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  yanliang (), fedoracdu@gmail.com
 *        Company:
 *
 * =====================================================================================
 */
#define FILE_SIZE 5120

#define BLURAY_PATH "BDMV/STREAM"

int bluray(const char *dir_name);

int xchdir(const char *path);
