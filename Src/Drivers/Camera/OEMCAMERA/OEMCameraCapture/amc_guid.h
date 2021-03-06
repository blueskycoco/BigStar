#ifndef __ARCSPFTCLSID_H__
#define __ARCSPFTCLSID_H__

///////////////////////////////////
//
//ArcSoft decoder ids
//
//////////////////////////////////

DEFINE_GUID(CLSID_ArcMpeg4Decoder, 
0x90348cbb, 0xd453, 0x4d8d, 0x34, 0x24, 0x34, 0x93, 0x89, 0x6, 0x33, 0x33);

// {3A21A6E8-31E3-467c-9428-A36549F5195E}
DEFINE_GUID(CLSID_ArcH263Decoder, 
0x3a21a6e8, 0x31e3, 0x467c, 0x94, 0x28, 0xa3, 0x65, 0x49, 0xf5, 0x19, 0x5e);

DEFINE_GUID(CLSID_ArcAmrnbDecoder, 
0x90341234, 0xd453, 0x4d8d, 0x34, 0x24, 0x34, 0x93, 0x89, 0x6, 0x33, 0x33);

// {9BD4BB61-79E3-4a22-BBF8-6E3DDB861EE4}
DEFINE_GUID(CLSID_ArcAmrwbDecoder, 
0x9bd4bb61, 0x79e3, 0x4a22, 0xbb, 0xf8, 0x6e, 0x3d, 0xdb, 0x86, 0x1e, 0xe4);

// {327E9F81-5931-499a-B4E1-24B1317709EA}
DEFINE_GUID(CLSID_ArcAACDecoder, 
0x327e9f81, 0x5931, 0x499a, 0xb4, 0xe1, 0x24, 0xb1, 0x31, 0x77, 0x9, 0xea);

// {3A2EA9AE-3323-436c-B5FD-A4B1EB882B27}
DEFINE_GUID(CLSID_ArcQCELPDecoder, 
0x3a2ea9ae, 0x3323, 0x436c, 0xb5, 0xfd, 0xa4, 0xb1, 0xeb, 0x88, 0x2b, 0x27);




//////////////////////////////////
//
//ArcSoft encoder ids
//
/////////////////////////////////

// {92CCEC70-263C-4ee3-B1BA-31439A23304E}
DEFINE_GUID(CLSID_ArcMpeg4Encoder, 
0x92ccec70, 0x263c, 0x4ee3, 0xb1, 0xba, 0x31, 0x43, 0x9a, 0x23, 0x30, 0x4e);

// {A4E3A85E-8AA8-413c-A5FD-84034B911FCC}
DEFINE_GUID(CLSID_ArcH263Encoder, 
0xa4e3a85e, 0x8aa8, 0x413c, 0xa5, 0xfd, 0x84, 0x3, 0x4b, 0x91, 0x1f, 0xcc);

// {38AFE4CA-FC78-4199-A1ED-5D9E65833DDE}
DEFINE_GUID(CLSID_ArcAmrnbEncoder, 
0x38afe4ca, 0xfc78, 0x4199, 0xa1, 0xed, 0x5d, 0x9e, 0x65, 0x83, 0x3d, 0xde);

// {664E3B32-643C-424b-B06D-204ADA851C69}
DEFINE_GUID(CLSID_ArcAmrwbEncoder, 
0x664e3b32, 0x643c, 0x424b, 0xb0, 0x6d, 0x20, 0x4a, 0xda, 0x85, 0x1c, 0x69);

// {8E49599C-731B-4af9-AC25-A3F274581875}
DEFINE_GUID(CLSID_ArcQCELPEncoder, 
0x8e49599c, 0x731b, 0x4af9, 0xac, 0x25, 0xa3, 0xf2, 0x74, 0x58, 0x18, 0x75);



////////////////////////////////
//
//ArcSoft source splitter ids
//
////////////////////////////////

// {21112234-D206-428f-9B77-A67210775770}
DEFINE_GUID(CLSID_ArcMidiSplitter, 
0x21112234, 0xd206, 0x428f, 0x9b, 0x77, 0xa6, 0x72, 0x10, 0x77, 0x57, 0x70);

// {1D78DD27-2230-4790-8DAC-933348D47B50}
DEFINE_GUID(CLSID_ArcAmrSourceSplitter, 
0x1d78dd27, 0x2230, 0x4790, 0x8d, 0xac, 0x93, 0x33, 0x48, 0xd4, 0x7b, 0x50);

// {07CA8887-6BA8-433d-A0BD-90E68998EF37}
DEFINE_GUID(CLSID_ArcMP4SourceSplitter, 
0x7ca8887, 0x6ba8, 0x433d, 0xa0, 0xbd, 0x90, 0xe6, 0x89, 0x98, 0xef, 0x37);

// {53B549AF-82DD-468a-B83B-FCCC09BACD08}
DEFINE_GUID(CLSID_ArcAACSourceSplitter, 
0x53b549af, 0x82dd, 0x468a, 0xb8, 0x3b, 0xfc, 0xcc, 0x9, 0xba, 0xcd, 0x8);

// {B2478371-E6C8-4936-96B2-C884AC616B6A}
DEFINE_GUID(CLSID_ArcQCELPSourceSplitter, 
0xb2478371, 0xe6c8, 0x4936, 0x96, 0xb2, 0xc8, 0x84, 0xac, 0x61, 0x6b, 0x6a);




///////////////////////////////
//
//ArcSoft muxer ids
//
//////////////////////////////

// {6A59220B-A696-4b79-AD80-E3C239A9D788}
DEFINE_GUID(CLSID_ArcAMRMuxRender, 
0x6a59220b, 0xa696, 0x4b79, 0xad, 0x80, 0xe3, 0xc2, 0x39, 0xa9, 0xd7, 0x88);

// {9F01F446-8EEE-40f0-AEA3-6F0A9B7652B9}
DEFINE_GUID(CLSID_ArcMP4MuxRender, 
0x9f01f446, 0x8eee, 0x40f0, 0xae, 0xa3, 0x6f, 0xa, 0x9b, 0x76, 0x52, 0xb9);

// {AFB87759-2D1F-4646-9347-BB3F165AA246}
DEFINE_GUID(CLSID_ArcQCELPMuxRender, 
0xafb87759, 0x2d1f, 0x4646, 0x93, 0x47, 0xbb, 0x3f, 0x16, 0x5a, 0xa2, 0x46);



//////////////////////////////
//
//Media subtype
//
/////////////////////////////

// {aa5e0668-2f56-48da-9e30-4f43270a00b1}
DEFINE_GUID(MEDIASUBTYPE_AMRNB, 
0xaa5e0668, 0x2f56, 0x48da, 0x9e, 0x30, 0x4f, 0x43, 0x27, 0x0a, 0x00, 0xb1);

// {A8D2C65D-822C-47f1-A2E9-41CF6064FC9E}
DEFINE_GUID(MEDIASUBTYPE_AMRWB, 
0xa8d2c65d, 0x822c, 0x47f1, 0xa2, 0xe9, 0x41, 0xcf, 0x60, 0x64, 0xfc, 0x9e);


#endif //__ARCSPFTCLSID_H__