﻿/* ***************************************************************************
 * file_service.c -- file service
 *
 * Copyright (C) 2016-2017 by Liu Chao <lc-soft@live.cn>
 *
 * This file is part of the LC-Finder project, and may only be used, modified,
 * and distributed under the terms of the GPLv2.
 *
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *
 * The LC-Finder project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for more details.
 *
 * You should have received a copy of the GPLv2 along with this file. It is
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ****************************************************************************/

/* ****************************************************************************
 * file_service.c -- 文件服务
 *
 * 版权所有 (C) 2016-2017 归属于 刘超 <lc-soft@live.cn>
 *
 * 这个文件是 LC-Finder 项目的一部分，并且只可以根据GPLv2许可协议来使用、更改和
 * 发布。
 *
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 *
 * LC-Finder 项目是基于使用目的而加以散布的，但不负任何担保责任，甚至没有适销
 * 性或特定用途的隐含担保，详情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在 LICENSE 文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>.
 * ****************************************************************************/

#ifndef LCFINDER_FILE_SERVICE_H
#define LCFINDER_FILE_SERVICE_H

LCFINDER_BEGIN_HEADER

#ifdef LCFINDER_FILE_SERVICE_C
typedef struct FileClientRec_* FileClient;
typedef struct FileStreamRec_* FileStream;
typedef struct ConnectionRec_* Connection;
#else
typedef void* FileClient;
typedef void* FileStream;
typedef void* Connection;
#endif

enum FileRequestMethod {
	REQUEST_METHOD_HEAD,
	REQUEST_METHOD_GET,
	REQUEST_METHOD_POST,
	REQUEST_METHOD_PUT,
	REQUEST_METHOD_DELETE
};

enum FileResponseStatus {
	RESPONSE_STATUS_OK = 200,
	RESPONSE_STATUS_BAD_REQUEST = 400,
	RESPONSE_STATUS_FORBIDDEN = 403,
	RESPONSE_STATUS_NOT_FOUND = 404,
	RESPONSE_STATUS_NOT_ACCEPTABLE = 406,
	RESPONSE_STATUS_ERROR = 500,
	RESPONSE_STATUS_NOT_IMPLEMENTED = 501
};

enum FileType {
	FILE_TYPE_ARCHIVE,
	FILE_TYPE_DIRECTORY
};

typedef struct FileImageStatus_ {
	unsigned int width;
	unsigned int height;
} FileImageStatus;

typedef struct FileStatus_ {
	int type;
	size_t size;
	time_t ctime;
	time_t mtime;
	FileImageStatus *image;
} FileStatus;

enum FileFilter {
	FILE_FILTER_NONE,
	FILE_FILTER_FILE,
	FILE_FILTER_FOLDER
};

/** 文件请求参数 */
typedef struct FileRequestParams_ {
	int filter;
	LCUI_BOOL get_thumbnail;
	LCUI_BOOL with_image_status;
	unsigned int width;
	unsigned int height;
} FileRequestParams;

typedef struct FileRequest_ {
	int method;
	wchar_t path[256];
	FileStatus file;
	FileRequestParams params;
	FileStream stream;
} FileRequest;

typedef struct FileResponse_ {
	int status;
	FileStatus file;
	FileStream stream;
} FileResponse;

typedef struct FileRequestHandler_ {
	void( *callback )(FileResponse*, void *);
	void *data;
} FileRequestHandler;

enum FileStreamChunkType {
	DATA_CHUNK_REQUEST,
	DATA_CHUNK_RESPONSE,
	DATA_CHUNK_BUFFER,
	DATA_CHUNK_THUMB,
	DATA_CHUNK_IMAGE,
	DATA_CHUNK_FILE,
	DATA_CHUNK_END
};

/** 文件流中的数据块 */
typedef struct FileStreamChunk_ {
	int type;			/**< 数据块类型 */
	union {
		FileRequest request;	/**< 文件请求 */
		FileResponse response;	/**< 文件请求的响应结果 */
		LCUI_Graph thumb;	/**< 缩略图 */
		LCUI_Graph image;	/**< 图像 */
		FILE *file;		/**< C 标准库的文件流 */
		char *data;		/**< 数据块 */
	};
	size_t cur;			/**< 当前游标位置 */
	size_t size;			/**< 数据总大小 */
} FileStreamChunk;

FileStream FileStream_Create( void );

void FileStream_Close( FileStream stream );

void FileStream_Destroy( FileStream stream );

size_t FileStream_ReadChunk( FileStream stream, FileStreamChunk *chunk );

size_t FileStream_WriteChunk( FileStream stream, FileStreamChunk *chunk );

size_t FileStream_Read( FileStream stream, char *buf,
			size_t size, size_t count );

size_t FileStream_Write( FileStream stream, char *buf,
			 size_t size, size_t count );

char *FileStream_ReadLine( FileStream stream, char *buf, size_t size );

Connection Connection_Create( void );

size_t Connection_Read( Connection conn, char *buf,
			size_t size, size_t count );

size_t Connection_Write( Connection conn, char *buf,
			 size_t size, size_t count );

int Connection_ReadChunk( Connection conn, FileStreamChunk *chunk );

int Connection_WriteChunk( Connection conn, FileStreamChunk *chunk );

void Connection_Close( Connection conn );

void Connection_Destroy( Connection conn );

void FileService_Run( void );

void FileService_RunAsync( void );

void FileService_Close( void );

void FileService_Init( void );

int Connection_SendRequest( Connection conn,
			    const FileRequest *request );

int Connection_ReceiveRequest( Connection conn,
			       FileRequest *request );

int Connection_SendResponse( Connection conn,
			     const FileResponse *response );

int Connection_ReceiveResponse( Connection conn,
				FileResponse *response );

FileClient FileClient_Create( void );

int FileClient_Connect( FileClient client );

void FileClient_Destroy( FileClient client );

void FileClient_Run( FileClient client );

void FileClient_Close( FileClient client );

void FileClient_RunAsync( FileClient client );

void FileClient_SendRequest( FileClient client,
			     const FileRequest *request,
			     const FileRequestHandler *handler );

LCFINDER_END_HEADER

#endif