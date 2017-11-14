#ifndef __STRING_CONVERTER_H__
#define __STRING_CONVERTER_H__

#include <string>
#include <sstream>

	    namespace StringConverter
	    {
		    /*========================================================================  
			    Name:       UTF8תUnicode
			    ----------------------------------------------------------  
			    Params:     [value]				UTF8�ַ���
		        ----------------------------------------------------------  
			    Return:    ����Unicode�ַ�����
		    ==========================================================================*/ 
		    std::wstring utf8_to_unicode(const std::string &value);

		    /*========================================================================  
			    Name:       UnicodeתUTF8
			    ----------------------------------------------------------  
			    Params:     [value]				Unicode�ַ���
		        ----------------------------------------------------------  
			    Return:    ����UTF8�ַ�����
		    ==========================================================================*/ 
		    std::string unicode_to_utf8(const std::wstring &value);

		    /*========================================================================  
			    Name:       ANSIתUnicode
			    ----------------------------------------------------------  
			    Params:     [value]				ANSI�ַ���
		        ----------------------------------------------------------  
			    Return:    ����Unicode�ַ�����
		    ==========================================================================*/ 
		    std::wstring ansi_to_unicode(const std::string &value);

		    /*========================================================================  
			    Name:       UnicodeתANSI
			    ----------------------------------------------------------  
			    Params:     [value]				Unicode�ַ���
		        ----------------------------------------------------------  
			    Return:    ����ANSI�ַ�����
		    ==========================================================================*/ 
		    std::string unicode_to_ansi(const std::wstring &value);

		    /*========================================================================  
			    Name:       ANSIתUTF8
			    ----------------------------------------------------------  
			    Params:     [value]				ANSI�ַ���
		        ----------------------------------------------------------  
			    Return:    ����UTF8�ַ�����
		    ==========================================================================*/ 
		    std::string ansi_to_utf8(const std::string &value);

		    /*========================================================================  
			    Name:       UTF8תANSI
			    ----------------------------------------------------------  
			    Params:     [value]				UTF8�ַ���
		        ----------------------------------------------------------  
			    Return:    ����ANSI�ַ�����
		    ==========================================================================*/ 
		    std::string utf8_to_ansi(const std::string &value);
		
		    /*========================================================================  
			    Name:       ���ַ���ת��Ϊ��������
			    ----------------------------------------------------------  
			    Params:     [value]				Դ�ַ���
		        ----------------------------------------------------------  
			    Return:    ����ת����Ķ���
		    ==========================================================================*/ 
		    template<class T>
		    T convert_string_to(const std::string &value);

		    /*========================================================================  
			    Name:       ����������ת��Ϊ�ַ���
			    ----------------------------------------------------------  
			    Params:     [value]				Դ����
		        ----------------------------------------------------------  
			    Return:    ����ת�����ַ�����
		    ==========================================================================*/ 
		    template<class T>
		    std::string convert_to_string(const T &value);

		    /*========================================================================  
			    Name:       URL����
			    ----------------------------------------------------------  
			    Params:     [value]				����ǰ���ַ���
		        ----------------------------------------------------------  
			    Return:    ���ر������ַ�����
		    ==========================================================================*/ 
		    std::string url_encode( const std::string &value );

		    /*========================================================================  
			    Name:       URL����
			    ----------------------------------------------------------  
			    Params:     [value]				�������ַ���
		        ----------------------------------------------------------  
			    Return:    ���ر���ǰ���ַ�����
		    ==========================================================================*/ 
		    std::string url_decode( const std::string &value );


		    template<class T>
		    T convert_string_to(const std::string &value)
		    {
                std::istringstream iss(value);
			    T value;
			    iss >> value;
			    return value;
		    }

		    template<class T>
		    std::string convert_to_string(const T &value)
		    {
			    std::ostringstream oss;
			    oss << value;
			    return oss.str();
		    }
        }

#endif