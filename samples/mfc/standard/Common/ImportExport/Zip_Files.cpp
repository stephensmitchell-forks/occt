/*
PARTIO SOFTWARE
Copyright 2010 Disney Enterprises, Inc. All rights reserved
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.
* The names "Disney", "Walt Disney Pictures", "Walt Disney Animation
Studios" or the names of its contributors may NOT be used to
endorse or promote products derived from this software without
specific prior written permission from Walt Disney Pictures.
Disclaimer: THIS SOFTWARE IS PROVIDED BY WALT DISNEY PICTURES AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE, NONINFRINGEMENT AND TITLE ARE DISCLAIMED.
IN NO EVENT SHALL WALT DISNEY PICTURES, THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#ifdef HAVE_ZLIB
extern "C"{
#include <zlib.h>
}
#endif

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <algorithm>

#include "Zip_Files.h"

template<class T>
inline void Read_Primitive(std::istream& stream, T& x)
{
	stream.read(&(char&)x, sizeof(T));
}

//#####################################################################
// class GZipFileHeader
//#####################################################################
struct GZipFileHeader
{
	unsigned char magic0, magic1; // magic should be 0x8b,0x1f
	unsigned char cm; // compression method 0x8 is gzip
	unsigned char flags; // flags
	unsigned int modtime; // 4 byte modification time
	unsigned char flags2; // secondary flags
	unsigned char os; // operating system 0xff for unknown
	unsigned short crc16; // crc check
	unsigned int crc32;

	GZipFileHeader()
		:magic0(0), magic1(0), flags(0), modtime(0), flags2(0), os(0), crc16(0), crc32(0)
	{}

	bool Read(std::istream& istream)
	{
		Read_Primitive(istream, magic0);
		Read_Primitive(istream, magic1);
		if (magic0 != 0x1f || magic1 != 0x8b){//std::cerr<<"gzip: did not find gzip magic 0x1f 0x8b"<<std::endl;
			return false;
		}
		Read_Primitive(istream, cm);
		if (cm != 8){ std::cerr << "gzip: compression method not 0x8" << std::endl; return false; }
		Read_Primitive(istream, flags);
		Read_Primitive(istream, modtime);
		Read_Primitive(istream, flags2);
		Read_Primitive(istream, os);
		unsigned char dummyByte;
		// read flags if necessary
		if (flags & 2){
			unsigned short flgExtraLen;
			Read_Primitive(istream, flgExtraLen);
			for (int k = 0; k<flgExtraLen; k++) Read_Primitive(istream, dummyByte);
		}
		// read filename/comment if present
		int stringsToRead = ((flags & 8) ? 1 : 0) + ((flags & 4) ? 1 : 0);
		for (int i = 0; i<stringsToRead; i++)
		do{ Read_Primitive(istream, dummyByte); } while (dummyByte != 0 && istream);
		if (flags & 1) Read_Primitive(istream, crc16);
		if (!istream) { std::cerr << "gzip: got to end of file after only reading gzip header" << std::endl; return false; }
		return true;
	}
};

#ifdef HAVE_ZLIB

//#####################################################################
// class ZipFileHeader
//#####################################################################
struct ZipFileHeader
{
	unsigned short version;
	unsigned short flags;
	unsigned short compression_type;
	unsigned short stamp_date, stamp_time;
	unsigned int crc;
	unsigned int compressed_size, uncompressed_size;
	std::string filename;
	unsigned int header_offset; // local header offset

	ZipFileHeader()
	{}

	ZipFileHeader(const std::string& filename_input)
		:version(20), flags(0), compression_type(8), stamp_date(0), stamp_time(0), crc(0),
		compressed_size(0), uncompressed_size(0), filename(filename_input), header_offset(0)
	{}

	bool Read(std::istream& istream, const bool global)
	{
		unsigned int sig;
		unsigned short version, flags;
		// read and check for local/global magic
		if (global){
			Read_Primitive(istream, sig);
			if (sig != 0x02014b50){ std::cerr << "Did not find global header signature" << std::endl; return false; }
			Read_Primitive(istream, version);
		}
		else{
			Read_Primitive(istream, sig);
			if (sig != 0x04034b50){ std::cerr << "Did not find local header signature" << std::endl; return false; }
		}
		// Read rest of header
		Read_Primitive(istream, version);
		Read_Primitive(istream, flags);
		Read_Primitive(istream, compression_type);
		Read_Primitive(istream, stamp_date);
		Read_Primitive(istream, stamp_time);
		Read_Primitive(istream, crc);
		Read_Primitive(istream, compressed_size);
		Read_Primitive(istream, uncompressed_size);
		unsigned short filename_length, extra_length;
		Read_Primitive(istream, filename_length);
		Read_Primitive(istream, extra_length);
		unsigned short comment_length = 0;
		if (global){
			Read_Primitive(istream, comment_length); // filecomment
			unsigned short disk_number_start, int_file_attrib;
			unsigned int ext_file_attrib;
			Read_Primitive(istream, disk_number_start); // disk# start
			Read_Primitive(istream, int_file_attrib); // internal file
			Read_Primitive(istream, ext_file_attrib); // ext final
			Read_Primitive(istream, header_offset);
		} // rel offset
		char* buf = new char[std::max(comment_length, std::max(filename_length, extra_length)) + 1];
		istream.read(buf, filename_length);
		buf[filename_length] = 0;
		filename = std::string(buf);
		istream.read(buf, extra_length);
		if (global) istream.read(buf, comment_length);
		delete[] buf;
		return true;
	}
};

//#####################################################################
// class ZipStreambufDecompress
//#####################################################################
class ZipStreambufDecompress :public std::streambuf
{
	static const unsigned int buffer_size = 512;
	std::istream& istream;

	z_stream strm;
	unsigned char in[buffer_size], out[buffer_size];
	ZipFileHeader header;
	GZipFileHeader gzip_header;
	int total_read, total_uncompressed;
	bool part_of_zip_file;
	bool own_istream;
	bool valid;
	bool compressed_data;

	static const unsigned short DEFLATE = 8;
	static const unsigned short UNCOMPRESSED = 0;
public:
	ZipStreambufDecompress(std::istream& stream, bool part_of_zip_file_input)
		:istream(stream), total_read(0), total_uncompressed(0), part_of_zip_file(part_of_zip_file_input), valid(true)
	{
		strm.zalloc = Z_NULL; strm.zfree = Z_NULL; strm.opaque = Z_NULL; strm.avail_in = 0; strm.next_in = Z_NULL;
		setg((char*)in, (char*)in, (char*)in);
		setp(0, 0);
		// skip the header
		if (part_of_zip_file){
			valid = header.Read(istream, false);
			if (header.compression_type == DEFLATE) compressed_data = true;
			else if (header.compression_type == UNCOMPRESSED) compressed_data = false;
			else{
				compressed_data = false; std::cerr << "ZIP: got unrecognized compressed data (Supported deflate/uncompressed)" << std::endl;
				valid = false;
			}
		}
		else{ valid = gzip_header.Read(istream); compressed_data = true; }
		// initialize the inflate
		if (compressed_data && valid){
			int result = inflateInit2(&strm, -MAX_WBITS);
			if (result != Z_OK){ std::cerr << "gzip: inflateInit2 did not return Z_OK" << std::endl; valid = false; }
		}
	}

	ZipStreambufDecompress & operator=(const ZipStreambufDecompress &) { return *this; }
	virtual ~ZipStreambufDecompress()
	{
		if (compressed_data && valid) inflateEnd(&strm);
		if (!part_of_zip_file) delete &istream;
	}

	int process()
	{
		if (!valid) return -1;
		if (compressed_data){
			strm.avail_out = buffer_size - 4;
			strm.next_out = (Bytef*)(out + 4);
			while (strm.avail_out != 0){
				if (strm.avail_in == 0){ // buffer empty, read some more from file
					istream.read((char*)in, part_of_zip_file ? std::min((unsigned int)buffer_size, header.compressed_size - total_read) : (unsigned int)buffer_size);
					strm.avail_in = (uInt)istream.gcount();
					total_read += strm.avail_in;
					strm.next_in = (Bytef*)in;
				}
				int ret = inflate(&strm, Z_NO_FLUSH); // decompress
				switch (ret){
				case Z_STREAM_ERROR:
					std::cerr << "libz error Z_STREAM_ERROR" << std::endl;
					valid = false; return -1;
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					std::cerr << "gzip error " << strm.msg << std::endl;
					valid = false; return -1;
				}
				if (ret == Z_STREAM_END) break;
			}
			int unzip_count = buffer_size - strm.avail_out - 4;
			total_uncompressed += unzip_count;
			return unzip_count;
		}
		else{ // uncompressed, so just read
			istream.read((char*)(out + 4), std::min(buffer_size - 4, header.uncompressed_size - total_read));
			int count = (int)istream.gcount();
			total_read += count;
			return count;
		}
	}

	virtual int underflow()
	{
		if (gptr() && (gptr()<egptr())) return traits_type::to_int_type(*gptr()); // if we already have data just use it
		int put_back_count = (int) (gptr() - eback());
		if (put_back_count>4) put_back_count = 4;
		std::memmove(out + (4 - put_back_count), gptr() - put_back_count, put_back_count);
		int num = process();
		setg((char*)(out + 4 - put_back_count), (char*)(out + 4), (char*)(out + 4 + num));
		if (num <= 0) return EOF;
		return traits_type::to_int_type(*gptr());
	}

	virtual int overflow()
	{
		assert(false); return EOF;
	}
};

//#####################################################################
// Class ZIP_FILE_ISTREAM
//#####################################################################
// Class needed because istream cannot own its streambuf
class ZIP_FILE_ISTREAM :public std::istream
{
	ZipStreambufDecompress buf;
public:
	ZIP_FILE_ISTREAM(std::istream& istream, bool part_of_zip_file)
		:std::istream(&buf), buf(istream, part_of_zip_file)
	{}

	virtual ~ZIP_FILE_ISTREAM()
	{}
};

//#####################################################################
// Function ZipFileReader
//#####################################################################
ZipFileReader::
ZipFileReader(std::ifstream& ifstream, const std::string& filename)
{
	ifstream.open(filename.c_str(), std::ios::in | std::ios::binary);
	if (!ifstream) throw std::runtime_error("ZIP: Invalid file handle");
	Find_And_Read_Central_Header(ifstream);
}
//#####################################################################
// Function ZipFileReader
//#####################################################################
ZipFileReader::
~ZipFileReader()
{
	std::map<std::string, ZipFileHeader*>::iterator i = filename_to_header.begin();
	for (; i != filename_to_header.end(); ++i)
		delete i->second;
}
//#####################################################################
// Function Find_And_Read_Central_Header
//#####################################################################
bool ZipFileReader::
Find_And_Read_Central_Header(std::ifstream& ifstream)
{
	// Find the header
	// NOTE: this assumes the zip file header is the last thing written to file...
	ifstream.seekg(0, std::ios_base::end);
	std::ios::streampos end_position = ifstream.tellg();
	unsigned int max_comment_size = 0xffff; // max size of header
	unsigned int read_size_before_comment = 22;
	std::ios::streamoff read_start = max_comment_size + read_size_before_comment;
	if (read_start>end_position) read_start = end_position;
	ifstream.seekg(end_position - read_start);
	char *buf = new char[read_start];
	if (read_start <= 0){ std::cerr << "ZIP: Invalid read buffer size" << std::endl; return false; }
	ifstream.read(buf, read_start);
	int found = -1;
	for (unsigned int i = 0; i<read_start - 3; i++){
		if (buf[i] == 0x50 && buf[i + 1] == 0x4b && buf[i + 2] == 0x05 && buf[i + 3] == 0x06){ found = i; break; }
	}
	delete[] buf;
	if (found == -1){ std::cerr << "ZIP: Failed to find zip header" << std::endl; return false; }
	// seek to end of central header and read
	ifstream.seekg(end_position - (read_start - found));
	unsigned int word;
	unsigned short disk_number1, disk_number2, num_files, num_files_this_disk;
	Read_Primitive(ifstream, word); // end of central
	Read_Primitive(ifstream, disk_number1); // this disk number
	Read_Primitive(ifstream, disk_number2); // this disk number
	if (disk_number1 != disk_number2 || disk_number1 != 0){
		std::cerr << "ZIP: multiple disk zip files are not supported" << std::endl; return false;
	}
	Read_Primitive(ifstream, num_files); // one entry in center in this disk
	Read_Primitive(ifstream, num_files_this_disk); // one entry in center 
	if (num_files != num_files_this_disk){
		std::cerr << "ZIP: multi disk zip files are not supported" << std::endl; return false;
	}
	unsigned int size_of_header, header_offset;
	Read_Primitive(ifstream, size_of_header); // size of header
	Read_Primitive(ifstream, header_offset); // offset to header
	// go to header and read all file headers
	ifstream.seekg(header_offset);
	for (int i = 0; i<num_files; i++){
		ZipFileHeader* header = new ZipFileHeader;
		bool valid = header->Read(ifstream, true);
		if (valid) filename_to_header[header->filename] = header;
	}
	return true;
}
//#####################################################################
// Function Get_File
//#####################################################################
std::istream* ZipFileReader::Get_File(std::ifstream& ifstream,const std::string& filename)
{
	std::map<std::string, ZipFileHeader*>::iterator i = filename_to_header.find(filename);
	if (i != filename_to_header.end()){
		ZipFileHeader* header = i->second;
		ifstream.seekg((*header).header_offset); return new ZIP_FILE_ISTREAM(ifstream, true);
	}
	return 0;
}
//#####################################################################
// Function Get_File_List
//#####################################################################
void ZipFileReader::Get_File_List(std::vector<std::string>& filenames) const
{
	filenames.clear();
	std::map<std::string, ZipFileHeader*>::const_iterator i = filename_to_header.begin();
	for (; i != filename_to_header.end(); ++i)
		filenames.push_back(i->first);
}

#else

ZipFileReader::
ZipFileReader(std::ifstream& ifstream, const std::string& filename)
{
  if (!ifstream) std::cerr << "Encountered stpZ file '" << filename << "' not compiled with zlib" << std::endl;
}

ZipFileReader::
~ZipFileReader()
{
}

std::istream* ZipFileReader::Get_File(std::ifstream& ifstream, const std::string& filename)
{
  if (!ifstream) std::cerr << "Encountered stpZ file '" << filename << "' not compiled with zlib" << std::endl;
  return 0;
}

void ZipFileReader::Get_File_List(std::vector<std::string>& filenames) const
{
  std::cerr << "Filenames empty because not compiled with zlib" << std::endl;
  filenames.clear();
}

#endif