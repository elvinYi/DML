#include <windows.h>
#include <vector>
#include <sstream>

#include "StringConverter.h"


	    namespace StringConverter
	    {
		    std::wstring utf8_to_unicode(const std::string &value)
		    {
			    wchar_t *result_buffer = NULL;
                std::wstring result_string;

			    int size = ::MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, NULL, 0);  
			    if (size != 0)  
			    {
				    result_buffer = new wchar_t[size];
				    ::MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, result_buffer, size);
                    result_string = result_buffer;
                    delete [] result_buffer;
			    }
			    return result_string;
		    }

		    std::string unicode_to_utf8(const std::wstring &value)
		    {
                char *result_buffer = NULL;
			    std::string result_string;
			    int size = ::WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, NULL, 0 , NULL , NULL);  
			    if (size != 0)
			    {
                    result_buffer = new char[size];
				    ::WideCharToMultiByte(CP_UTF8, 0, value.c_str(), -1, result_buffer, size, NULL, NULL);
                    result_string = result_buffer;
                    delete [] result_buffer;
			    }
			    return result_string;
		    }

		    std::wstring ansi_to_unicode(const std::string &value)
		    {
                wchar_t *result_buffer = NULL;
			    std::wstring result_string;
			    int size = ::MultiByteToWideChar(CP_ACP, 0, value.c_str(), -1, NULL, 0);  
			    if (size != 0)  
			    {
				    result_buffer = new wchar_t[size];
				    ::MultiByteToWideChar(CP_ACP, 0, value.c_str(), -1, result_buffer, size);
                    result_string = result_buffer;
                    delete [] result_buffer;
			    }
			    return result_string;
		    }

		    std::string unicode_to_ansi(const std::wstring &value)
		    {
                char *result_buffer = NULL;
                std::string result_string;
			    int size = ::WideCharToMultiByte(CP_ACP, 0, value.c_str(), -1, NULL, 0 , NULL, NULL);  
			    if (size != 0)
			    {
				    result_buffer = new char[size];
				    ::WideCharToMultiByte(CP_ACP, 0, value.c_str(), -1, result_buffer, size, NULL, NULL);
                    result_string = result_buffer;
                    delete [] result_buffer;
			    }
			    return result_string;
		    }

		    std::string ansi_to_utf8(const std::string &value)
		    {
			    std::wstring unicode = ansi_to_unicode(value);
			    return unicode_to_utf8(unicode);
		    }

		    std::string utf8_to_ansi(const std::string &value)
		    {
			    std::wstring unicode = utf8_to_unicode(value);
			    return unicode_to_ansi(unicode);
		    }

		    std::string url_encode(const std::string &value)  
		    {  
			    static const char *digits = "0123456789ABCDEF";
			    std::string strOut;  
			    for ( std::string::const_iterator iter = value.begin(); iter != value.end(); ++iter )  
			    {
				    unsigned char chCurrent = static_cast <unsigned char> ( *iter );
				    unsigned char szBuffer[4];  
				    memset( szBuffer, 0, sizeof( szBuffer ) );  
				    if( isalnum( chCurrent ) )
				    {        
					    szBuffer[0] = chCurrent;  
				    }  
				    else  
				    {
					    szBuffer[0] = '%';  
					    szBuffer[1] = digits[ ( chCurrent >> 4 ) & 0x0F  ];
					    szBuffer[2] = digits[          chCurrent & 0x0F  ];
				    }  
				    strOut += (char *)szBuffer;
			    }  
			    return strOut;  
		    }
	  
		    std::string url_decode(const std::string &value)  
		    {  
			    std::string strOut;  
			    for ( std::string::const_iterator iter = value.begin(); iter != value.end(); ++iter )  
			    {
				    unsigned char chCurrent = 0;  
				    if ( *iter == '%' )
				    {
					    if ( (iter + 1 != value.end()) && (iter + 2 != value.end()) )
					    {
						    unsigned char chLeft  = *(iter + 1);
						    unsigned char chRight = *(iter + 2);
						    chCurrent = ( isdigit( chLeft ) ? chLeft - '0' : chLeft - 'A' + 10 ) << 4;
						    chCurrent |= isdigit( chRight ) ? chRight - '0' : chRight - 'A'+10; 
						    iter += 2;
					    }
					    else
					    {
						    chCurrent = '?';
					    }
				    }  
				    else if ( *iter == '+')
				    {
					    chCurrent = ' ';
				    }
				    else
				    {
					    chCurrent = *iter;  
				    }
				    strOut += chCurrent;  
			    }
			    return strOut;  
	    	}
    	}
