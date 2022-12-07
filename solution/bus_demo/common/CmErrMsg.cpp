/*************************************************************************
 *
 *  CmErrMsg.cpp
 *
 * Copyright(c) 2011-2012 Renesas Electronics Corporation
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation. No part of this
 * program may be reproduced or disclosed to others, in any
 * form, without the prior written permission of Renesas Electronics
 * Corporation.
 *
 *************************************************************************/

/* $Id:$ */

#include "CmErrMsg.h"


namespace CmErrMsg{
  namespace CmErrMsgPrivate{
    char ErrMsgFile[1024] = "ErrMsg.txt";
    bool alreadySet = false;
#ifdef WINDOWS_DEF
    bool toFile = true;
#else  //WINDOWS_DEF
    bool toFile = false;
#endif //WINDOWS_DEF
    void ErrMsgFileOpenError(const char *);
  }  
}


/// <summary>initialize CmErrMsg</summary>
/// <param name="ConfigFile">configration file name</param>
void CmErrMsg::CmErrMsgInitSet(const char *ConfigFile)
{
  
  char	  word[512];
  char	  seps[] = " ,=();\t\n\r";
  char	  *token;
  
  if( !CmErrMsgPrivate::alreadySet ){
    
    CmErrMsgPrivate::alreadySet = true;
    
    ifstream  config_file(ConfigFile);
    
    if( config_file.fail() ){
      CmErrMsgPrivate::ErrMsgFileOpenError( ConfigFile );
    }
    else{
      // read the config file
      while(1){
	config_file.getline( word, 512, '\n' );
	token = strtok( word, seps );
	
	// ----------------------------------------
	// For EOF
	if ( config_file.eof() ){
	  break;
	}
	
	// ----------------------------------------
	// For comment
	if( ( token == NULL ) || ( strncmp(token, "//", 2) == 0 ) ){
	  continue;
	}
	
	// ----------------------------------------
	// detect end marker
	// NOTICE:You have to check whether token is NULL first.
	if ( strcmp(token, "[END]") == 0 ){
	  break;
	}
	
	// ----------------------------------------
	// get token
	// ----------------------------------------
	// For miscellaneous
	if( strcmp(token, "[ERROR_MESSAGE_FILE]") == 0 ){
	  token = strtok(0, seps);
	  if ( token != NULL ){
	    strcpy( CmErrMsgPrivate::ErrMsgFile, token );
	    CmErrMsgPrivate::toFile = true;
	  }
	  break;
	}
      }
      if( CmErrMsgPrivate::toFile ){
	std::fstream fStrm( CmErrMsgPrivate::ErrMsgFile, std::ios::out|std::ios::app );
	if( !fStrm.fail() ){
	  fStrm.close();
	  remove( CmErrMsgPrivate::ErrMsgFile );
	}
      }
    }
  }
}


/// <summary>print error message</summary>
/// <param name="moduleName">module name</param>
/// <param name="funcName">function name</param>
/// <param name="message">message</param>
void CmErrMsg::printErrorMessage(const char *moduleName, 
				 const char *funcName, 
				 const char *message)
{
  if( CmErrMsgPrivate::toFile == true ){
    std::fstream fStrm( CmErrMsgPrivate::ErrMsgFile, std::ios::out|std::ios::app );
    if( fStrm.fail() ){
      CmErrMsgPrivate::ErrMsgFileOpenError( CmErrMsgPrivate::ErrMsgFile );
    }
    else{
      fStrm << "Error : @" << sc_time_stamp().to_string() << " : " << moduleName << endl;
      if( funcName != NULL )
	fStrm << "\t[ Function ] " << funcName << endl;
      fStrm << "\t[ Message  ] " << message  << endl;
      fStrm << endl;
    }
  }
  else{
    cout << "Error : @" << sc_time_stamp().to_string() << " : " << moduleName << endl;
    if( funcName != NULL )
      cout << "\t[ Function ] " << funcName << endl;
    cout << "\t[ Message  ] " << message  << endl;
    cout << endl;
  }  
  
  if( scstopDoneFlag == 0 ){
    sc_stop();
    scstopDoneFlag = 1;
  }
  
}


/// <summary>print warning message</summary>
/// <param name="moduleName">module name</param>
/// <param name="funcName">function name</param>
/// <param name="message">message</param>
void CmErrMsg::printWarningMessage(const char *moduleName, 
				   const char *funcName, 
				   const char *message)
{
  if( CmErrMsgPrivate::toFile == true ){
    std::fstream fStrm( CmErrMsgPrivate::ErrMsgFile, std::ios::out|std::ios::app );
    if( fStrm.fail() ){
      CmErrMsgPrivate::ErrMsgFileOpenError( CmErrMsgPrivate::ErrMsgFile );
    }
    else{
      fStrm << "Warning : @" << sc_time_stamp().to_string() << " : " << moduleName << endl;
      if( funcName != NULL )
	fStrm << "\t[ Function ] " << funcName << endl;
      fStrm << "\t[ Message  ] " << message  << endl;
      fStrm << endl;
    }
  }
  else{
    cout << "Warning : @" << sc_time_stamp().to_string() << " : " << moduleName << endl;
    if( funcName != NULL )
      cout << "\t[ Function ] " << funcName << endl;
    cout << "\t[ Message  ] " << message  << endl;
    cout << endl;
  }
}


/// <summary>print error message output file open error</summary>
/// <param name="OpenErrorFile">file name</param>
void CmErrMsg::CmErrMsgPrivate::ErrMsgFileOpenError(const char *OpenErrorFile){
#ifdef WINDOWS_DEF
  std::fstream fStrm( "ErrMsg.txt", std::ios::out );
  fStrm << "Error : @" << sc_time_stamp().to_string() << " : CmErrMsg" << endl;
  fStrm << "\t[ Message  ] file open error!! : " << OpenErrorFile << endl;
  fStrm << endl;
#else  //WINDOWS_DEF
  cout << "Error : @" << sc_time_stamp().to_string() << " : CmErrMsg" << endl;
  cout << "\t[ Message  ] file open error!! : " << OpenErrorFile << endl;
  cout << endl;
#endif //WINDOWS_DEF
  
  exit(1);
  
}

/// <summary>set where message is output</summary>
/// <param name="flag">file or stdout</param>
void CmErrMsg::CmErrMsgSetToFile(bool flag)	{
  CmErrMsgPrivate::toFile = flag;
}

