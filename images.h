//Copyright Notice
/*
    eXintegrator integrated expression analysis system
    Copyright (C) 2004  Martin Jakt & Okada Mitsuhiro
  
    This file is part of the eXintegrator integrated expression analysis system. 
    eXintegrator is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version. 

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    PS. If you can think of a better name, please let me know...
*/
//End Copyright Notice

#ifndef _QEMBED_1804289383
#define _QEMBED_1804289383
#include <qimage.h>
#include <qdict.h>
static const unsigned char active_2_25_data[] = {
    0x01,0x01,0x01,0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
    0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x00,0x00,0x00,
    0x01,0x01,0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,
    0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x00,0x00,0x00,
    0x01,0x01,0x01,0x02,0x17,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
    0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x18,0x00,0x00,0x00,
    0x01,0x01,0x02,0x17,0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x0b,0x0c,0x0d,
    0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x18,0x1a,0x00,0x00,0x00,
    0x01,0x02,0x17,0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
    0x0f,0x10,0x11,0x12,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x00,0x00,0x00,
    0x02,0x17,0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x22,0x23,0x24,0x25,0x26,0x20,0x21,0x27,0x00,0x00,0x00,
    0x17,0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x28,0x0c,0x0d,0x0e,0x0f,0x10,
    0x29,0x2a,0x2b,0x2c,0x2d,0x16,0x18,0x1a,0x21,0x27,0x2e,0x00,0x00,0x00,
    0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x28,0x0c,0x0d,0x0e,0x0f,0x10,0x2f,
    0x30,0x31,0x14,0x15,0x16,0x18,0x1a,0x21,0x27,0x2e,0x32,0x00,0x00,0x00,
    0x05,0x06,0x19,0x08,0x09,0x0a,0x28,0x0c,0x0d,0x33,0x0f,0x10,0x34,0x35,
    0x36,0x14,0x15,0x16,0x18,0x1a,0x21,0x27,0x2e,0x32,0x37,0x00,0x00,0x00,
    0x06,0x19,0x08,0x09,0x0a,0x28,0x0c,0x0d,0x33,0x0f,0x10,0x38,0x39,0x3a,
    0x3b,0x15,0x16,0x18,0x1a,0x21,0x27,0x2e,0x32,0x37,0x3c,0x00,0x00,0x00,
    0x19,0x08,0x09,0x0a,0x28,0x0c,0x0d,0x33,0x0f,0x10,0x3d,0x3e,0x3f,0x3b,
    0x15,0x16,0x18,0x1a,0x21,0x27,0x2e,0x32,0x37,0x3c,0x40,0x00,0x00,0x00,
    0x08,0x09,0x0a,0x28,0x0c,0x41,0x42,0x43,0x10,0x44,0x45,0x46,0x47,0x15,
    0x16,0x18,0x1a,0x21,0x27,0x2e,0x32,0x37,0x3c,0x40,0x48,0x00,0x00,0x00,
    0x49,0x0a,0x28,0x0c,0x4a,0x4b,0x4c,0x4d,0x11,0x4e,0x4c,0x4f,0x15,0x16,
    0x18,0x1a,0x21,0x27,0x2e,0x32,0x37,0x3c,0x40,0x48,0x50,0x00,0x00,0x00,
    0x0a,0x28,0x0c,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x16,0x18,
    0x1a,0x21,0x27,0x2e,0x32,0x37,0x3c,0x40,0x48,0x50,0x5a,0x00,0x00,0x00,
    0x28,0x0c,0x0d,0x5b,0x5c,0x5d,0x5e,0x54,0x5f,0x60,0x61,0x16,0x18,0x1a,
    0x21,0x27,0x2e,0x32,0x37,0x3c,0x40,0x48,0x50,0x5a,0x62,0x00,0x00,0x00,
    0x0c,0x0d,0x33,0x63,0x64,0x11,0x65,0x54,0x66,0x60,0x67,0x18,0x1a,0x68,
    0x27,0x2e,0x32,0x37,0x3c,0x40,0x48,0x69,0x5a,0x62,0x6a,0x00,0x00,0x00,
    0x0d,0x33,0x0f,0x10,0x11,0x12,0x6b,0x54,0x54,0x6c,0x18,0x1a,0x68,0x27,
    0x2e,0x32,0x37,0x3c,0x40,0x48,0x69,0x5a,0x62,0x6a,0x6d,0x00,0x00,0x00,
    0x33,0x0f,0x10,0x11,0x12,0x1b,0x6e,0x54,0x54,0x6f,0x1a,0x68,0x27,0x2e,
    0x32,0x37,0x3c,0x40,0x48,0x69,0x5a,0x62,0x6a,0x6d,0x70,0x00,0x00,0x00,
    0x0f,0x71,0x11,0x12,0x1b,0x72,0x73,0x4c,0x74,0x75,0x68,0x27,0x2e,0x32,
    0x37,0x3c,0x40,0x48,0x69,0x5a,0x62,0x6a,0x6d,0x70,0x76,0x00,0x00,0x00,
    0x71,0x11,0x12,0x1b,0x77,0x15,0x78,0x79,0x7a,0x7b,0x27,0x2e,0x32,0x37,
    0x3c,0x40,0x48,0x69,0x5a,0x62,0x6a,0x6d,0x70,0x76,0x7c,0x00,0x00,0x00,
    0x11,0x12,0x1b,0x77,0x15,0x16,0x7d,0x7e,0x7f,0x27,0x2e,0x32,0x80,0x3c,
    0x40,0x48,0x69,0x5a,0x62,0x6a,0x6d,0x70,0x76,0x7c,0x81,0x00,0x00,0x00,
    0x12,0x1b,0x77,0x15,0x16,0x18,0x1a,0x68,0x27,0x2e,0x32,0x80,0x3c,0x40,
    0x48,0x69,0x5a,0x62,0x6a,0x6d,0x70,0x76,0x7c,0x81,0x82,0x00,0x00,0x00,
    0x1b,0x77,0x15,0x16,0x18,0x1a,0x68,0x27,0x2e,0x32,0x80,0x3c,0x40,0x48,
    0x69,0x5a,0x62,0x6a,0x6d,0x70,0x76,0x7c,0x81,0x83,0x84,0x00,0x00,0x00,
    0x77,0x15,0x16,0x18,0x1a,0x68,0x27,0x2e,0x32,0x80,0x3c,0x40,0x48,0x69,
    0x5a,0x62,0x85,0x6d,0x70,0x76,0x7c,0x81,0x83,0x84,0x86,0x00,0x00,0x00,
    0x15,0x16,0x18,0x1a,0x68,0x27,0x2e,0x32,0x80,0x3c,0x40,0x48,0x69,0x5a,
    0x62,0x85,0x6d,0x70,0x76,0x7c,0x81,0x83,0x84,0x86,0x87,0x00,0x00,0x00
};

static const QRgb active_2_25_ctable[] = {
    0xc6c6c6,0xff3b746e,0xff3d766e,0xff407970,0xff427b71,0xff447d72,0xff467f73,0xff498174,0xff4b8375,0xff4d8676,0xff4f8877,0xff518a79,0xff548c7a,0xff568e7b,
    0xff58917c,0xff5a937d,0xff5d957e,0xff5f977f,0xff619980,0xff639c81,0xff669e83,0xff68a084,0xff6aa285,0xff407870,0xff6ca486,0xff488174,0xff6ea687,0xff639b81,
    0xff6a9381,0xff806a7d,0xff8d517b,0xff807480,0xff6fa687,0xff71a988,0xff75787e,0xff89547b,0xff8e4c7a,0xff885d7d,0xff7a8482,0xff73ab89,0xff518a78,0xff5f967f,
    0xff74757d,0xff825f7b,0xff767c7f,0xff689f84,0xff75ad8a,0xff6b817d,0xff84577a,0xff6d887f,0xff77af8b,0xff58907c,0xff78697b,0xff923d78,0xff659881,0xff7ab18d,
    0xff756f7b,0xff9d2876,0xff699180,0xff659e83,0xff7cb38e,0xff69847d,0xffa21e75,0xff747a7e,0xff7eb68f,0xff588a7a,0xff68777a,0xff5b917d,0xff60967f,0xff973377,
    0xff953978,0xff669d82,0xff80b890,0xff4d8576,0xff706678,0xff9f2175,0xffa71574,0xff845078,0xff74747c,0xff79757e,0xff83ba91,0xff637a79,0xffa61674,0xffa31c74,
    0xffa81474,0xffa41b74,0xff6a887f,0xff933b78,0xff9f2776,0xff699c83,0xff85bc92,0xff775f78,0xff9b2a76,0xff72707b,0xffa02175,0xff89507a,0xffa61774,0xff85617c,
    0xff87be93,0xff60887c,0xff5d937e,0xff8c4678,0xffa61874,0xff719183,0xff71a888,0xff82ba91,0xff89c194,0xff7e657c,0xff973b79,0xff8bc395,0xff77797e,0xff85697e,
    0xff8ec597,0xff5c957e,0xff659e82,0xff748380,0xffa71674,0xff769384,0xff90c798,0xff659d82,0xff768581,0xffa81574,0xff9f2b77,0xff71a788,0xff92c999,0xff7a8181,
    0xff8f547c,0xff759c86,0xff79b18c,0xff94cb9a,0xff97ce9b,0xff96ce9b,0xff99d09c,0xff89c094,0xff9bd29d,0xff9cd29e
};

/* Generated by qembed */
static const unsigned char inactive_25_data[] = {
    0x01,0x01,0x01,0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
    0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x00,0x00,0x00,
    0x01,0x01,0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,
    0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x00,0x00,0x00,
    0x01,0x01,0x01,0x02,0x17,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,
    0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x18,0x00,0x00,0x00,
    0x01,0x01,0x02,0x17,0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x0b,0x0c,0x0d,
    0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x18,0x1a,0x00,0x00,0x00,
    0x01,0x02,0x17,0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
    0x0f,0x10,0x11,0x12,0x1b,0x14,0x15,0x16,0x18,0x1a,0x1c,0x00,0x00,0x00,
    0x02,0x17,0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x1b,0x14,0x15,0x16,0x18,0x1a,0x1c,0x1d,0x00,0x00,0x00,
    0x17,0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x1e,0x0c,0x0d,0x0e,0x0f,0x10,
    0x11,0x12,0x1b,0x14,0x15,0x16,0x18,0x1a,0x1c,0x1d,0x1f,0x00,0x00,0x00,
    0x04,0x05,0x06,0x19,0x08,0x09,0x0a,0x1e,0x0c,0x0d,0x0e,0x0f,0x10,0x11,
    0x12,0x1b,0x14,0x15,0x16,0x18,0x1a,0x1c,0x1d,0x1f,0x20,0x00,0x00,0x00,
    0x05,0x06,0x19,0x08,0x09,0x0a,0x1e,0x0c,0x0d,0x21,0x0f,0x10,0x11,0x12,
    0x1b,0x14,0x15,0x16,0x18,0x1a,0x1c,0x1d,0x1f,0x20,0x22,0x00,0x00,0x00,
    0x06,0x19,0x08,0x09,0x0a,0x1e,0x0c,0x0d,0x21,0x0f,0x10,0x11,0x12,0x1b,
    0x23,0x15,0x16,0x18,0x1a,0x1c,0x1d,0x1f,0x20,0x22,0x24,0x00,0x00,0x00,
    0x19,0x08,0x09,0x0a,0x1e,0x0c,0x0d,0x21,0x0f,0x10,0x11,0x12,0x1b,0x23,
    0x15,0x16,0x18,0x1a,0x1c,0x1d,0x1f,0x20,0x22,0x24,0x25,0x00,0x00,0x00,
    0x08,0x09,0x0a,0x1e,0x0c,0x0d,0x21,0x0f,0x10,0x11,0x12,0x1b,0x23,0x15,
    0x16,0x18,0x1a,0x1c,0x1d,0x1f,0x20,0x22,0x24,0x25,0x26,0x00,0x00,0x00,
    0x27,0x0a,0x1e,0x0c,0x0d,0x21,0x0f,0x10,0x11,0x12,0x1b,0x23,0x15,0x16,
    0x18,0x1a,0x1c,0x1d,0x1f,0x20,0x22,0x24,0x25,0x26,0x28,0x00,0x00,0x00,
    0x0a,0x1e,0x0c,0x0d,0x21,0x0f,0x10,0x11,0x12,0x1b,0x29,0x15,0x16,0x18,
    0x1a,0x1c,0x1d,0x1f,0x20,0x22,0x24,0x25,0x26,0x28,0x2a,0x00,0x00,0x00,
    0x1e,0x0c,0x0d,0x21,0x0f,0x10,0x11,0x12,0x1b,0x29,0x15,0x16,0x18,0x1a,
    0x1c,0x1d,0x1f,0x20,0x22,0x24,0x25,0x26,0x28,0x2a,0x2b,0x00,0x00,0x00,
    0x0c,0x0d,0x21,0x0f,0x10,0x11,0x12,0x1b,0x29,0x15,0x16,0x18,0x1a,0x2c,
    0x1d,0x1f,0x20,0x22,0x24,0x25,0x26,0x2d,0x2a,0x2b,0x2e,0x00,0x00,0x00,
    0x0d,0x21,0x0f,0x10,0x11,0x12,0x1b,0x29,0x15,0x16,0x18,0x1a,0x2c,0x1d,
    0x1f,0x20,0x22,0x24,0x25,0x26,0x2d,0x2a,0x2b,0x2e,0x2f,0x00,0x00,0x00,
    0x21,0x0f,0x10,0x11,0x12,0x1b,0x29,0x15,0x16,0x18,0x1a,0x2c,0x1d,0x1f,
    0x20,0x22,0x24,0x25,0x26,0x2d,0x2a,0x2b,0x2e,0x2f,0x30,0x00,0x00,0x00,
    0x0f,0x31,0x11,0x12,0x1b,0x29,0x15,0x16,0x18,0x1a,0x2c,0x1d,0x1f,0x20,
    0x22,0x24,0x25,0x26,0x2d,0x2a,0x2b,0x2e,0x2f,0x30,0x32,0x00,0x00,0x00,
    0x31,0x11,0x12,0x1b,0x33,0x15,0x16,0x18,0x1a,0x2c,0x1d,0x1f,0x20,0x22,
    0x24,0x25,0x26,0x2d,0x2a,0x2b,0x2e,0x2f,0x30,0x32,0x34,0x00,0x00,0x00,
    0x11,0x12,0x1b,0x33,0x15,0x16,0x18,0x1a,0x2c,0x1d,0x1f,0x20,0x35,0x24,
    0x25,0x26,0x2d,0x2a,0x2b,0x2e,0x2f,0x30,0x32,0x34,0x36,0x00,0x00,0x00,
    0x12,0x1b,0x33,0x15,0x16,0x18,0x1a,0x2c,0x1d,0x1f,0x20,0x35,0x24,0x25,
    0x26,0x2d,0x2a,0x2b,0x2e,0x2f,0x30,0x32,0x34,0x36,0x37,0x00,0x00,0x00,
    0x1b,0x33,0x15,0x16,0x18,0x1a,0x2c,0x1d,0x1f,0x20,0x35,0x24,0x25,0x26,
    0x2d,0x2a,0x2b,0x2e,0x2f,0x30,0x32,0x34,0x36,0x38,0x39,0x00,0x00,0x00,
    0x33,0x15,0x16,0x18,0x1a,0x2c,0x1d,0x1f,0x20,0x35,0x24,0x25,0x26,0x2d,
    0x2a,0x2b,0x3a,0x2f,0x30,0x32,0x34,0x36,0x38,0x39,0x3b,0x00,0x00,0x00,
    0x15,0x16,0x18,0x1a,0x2c,0x1d,0x1f,0x20,0x35,0x24,0x25,0x26,0x2d,0x2a,
    0x2b,0x3a,0x2f,0x30,0x32,0x34,0x36,0x38,0x39,0x3b,0x3c,0x00,0x00,0x00
};

static const QRgb inactive_25_ctable[] = {
    0xc6c6c6,0xff3b746e,0xff3d766e,0xff407970,0xff427b71,0xff447d72,0xff467f73,0xff498174,0xff4b8375,0xff4d8676,0xff4f8877,0xff518a79,0xff548c7a,0xff568e7b,
    0xff58917c,0xff5a937d,0xff5d957e,0xff5f977f,0xff619980,0xff639c81,0xff669e83,0xff68a084,0xff6aa285,0xff407870,0xff6ca486,0xff488174,0xff6ea687,0xff639b81,
    0xff71a988,0xff73ab89,0xff518a78,0xff75ad8a,0xff77af8b,0xff58907c,0xff7ab18d,0xff659e83,0xff7cb38e,0xff7eb68f,0xff80b890,0xff4d8576,0xff83ba91,0xff659e82,
    0xff85bc92,0xff87be93,0xff71a888,0xff82ba91,0xff89c194,0xff8bc395,0xff8ec597,0xff5c957e,0xff90c798,0xff659d82,0xff92c999,0xff79b18c,0xff94cb9a,0xff97ce9b,
    0xff96ce9b,0xff99d09c,0xff89c094,0xff9bd29d,0xff9cd29e
};

static struct EmbedImage {
    int width, height, depth;
    const unsigned char *data;
    int numColors;
    const QRgb *colorTable;
    bool alpha;
    const char *name;
} embed_image_vec[] = {
    { 25, 25, 8, (const unsigned char*)active_2_25_data, 136, active_2_25_ctable, TRUE, "active_2_25" },
    { 25, 25, 8, (const unsigned char*)inactive_25_data, 61, inactive_25_ctable, TRUE, "inactive_25" },
    { 0, 0, 0, 0, 0, 0, 0, 0 }
};

static const QImage& qembed_findImage( const QString& name )
{
    static QDict<QImage> dict;
    QImage* img = dict.find( name );
    if ( !img ) {
	for ( int i = 0; embed_image_vec[i].data; i++ ) {
	    if ( strcmp(embed_image_vec[i].name, name.latin1()) == 0 ) {
		img = new QImage((uchar*)embed_image_vec[i].data,
			    embed_image_vec[i].width,
			    embed_image_vec[i].height,
			    embed_image_vec[i].depth,
			    (QRgb*)embed_image_vec[i].colorTable,
			    embed_image_vec[i].numColors,
			    QImage::BigEndian );
		if ( embed_image_vec[i].alpha )
		    img->setAlphaBuffer( TRUE );
		dict.insert( name, img );
		break;
	    }
	}
	if ( !img ) {
	    static QImage dummy;
	    return dummy;
	}
    }
    return *img;
}

#endif
