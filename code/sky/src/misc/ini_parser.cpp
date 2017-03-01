#include "ini_parser.h"
#include <iostream>

void IniParser::Parse(istream& in)
{
    char ch;
    while (in.get(ch) && !Parse(ch))
      ;
    End();
}

bool IniParser::Parse(char ch)
{
    bool ret = false;
    switch (_state)
    {
        case STATE_0:
        {
            if (ch == '[')
            {
                _state = STATE_SECTION;
            }
            else if (isalnum(ch))
            {
                _data = ch;
                _state = STATE_KEY;
            }
            else if (ch == '#')
            {
                _state = STATE_COMMENT;
            }
            else if (isspace(ch))
            {
            }
            else
            {
                _data.push_back(ch);
                ret = _event.OnError(_data);
            }
            break;
        }

        case STATE_SECTION:
        {
            if (ch == ']')
            {
                ret = _event.OnSection(_data);
                _data.clear();
                _state = STATE_0;
            }
            else
            {
                _data += ch;
            }
            break;
        }

        case STATE_KEY:
        {
            if (ch == '=')
            {
                ret = _event.OnKey(_data);
                _state = STATE_VALUE0;
            }
            else if (isspace(ch))
            {
                ret = _event.OnKey(_data);
                _state = STATE_KEY_SP;
            }
            else if ((ch == '\n') || (ch == '\r'))
            {
                ret = _event.OnError(_data);
            }
            else
            {
                _data += ch;
            }
            break;
        }

        case STATE_KEY_SP:
        {
            if (ch == '=')
            {
                _state = STATE_VALUE0;
            }
            else if (!isspace(ch))
            {
                _data.push_back(ch);
                ret = _event.OnError(_data);
            }
            break;
        }

        case STATE_VALUE0:
        {
            if ((ch == '\n') || (ch == '\r'))
            {
                ret = _event.OnValue(string());
                _state = STATE_0;
            }
            else if (!isspace(ch))
            {
                _data = ch;
                _state = STATE_VALUE;
            }
            break;
        }

        case STATE_VALUE:
        {
            if ((ch == '\n') || (ch == '\r'))
            {
                ret = _event.OnValue(_data);
                _data.clear();
                _state = STATE_0;
            }
            else if (isspace(ch))
            {
                _state = STATE_VALUE_SP;
            }
            else
            {
                _data += ch;
            }
            break;
        }
        case STATE_VALUE_SP:
        {
            if ((ch == '\n') || (ch == '\r'))
            {
                ret = _event.OnValue(_data);
                _data.clear();
                _state = STATE_0;
            }
            else if (!isspace(ch))
            {   
                _data.push_back(ch);
                ret = _event.OnError(_data);
            }
            break;
        }

        case STATE_COMMENT:
        {
            if ((ch == '\n') || (ch == '\r'))
            {
                _state = STATE_0;
            }
            break;
        }

        default:
        {
            break;
        }
    }

    return ret;
}

void IniParser::End()
{
    switch (_state)
    {
        case STATE_0:
        case STATE_COMMENT:
        {
            break;
        }
        
        case STATE_SECTION:
        case STATE_KEY:
        case STATE_KEY_SP:
        {
            _event.OnError();
            break;
        }
        
        case STATE_VALUE0:
        {
            _event.OnValue(string());
            break;
        }
        
        case STATE_VALUE:
        {
            _event.OnValue(_data);
            break;
        }
        
        default:
        {
            break;
        }
    }
}

