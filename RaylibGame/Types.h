#pragma once
#include <ostream>

namespace Types {

    class  SString {
    protected:
        char* value = nullptr;
    public:
        SString()
        {
            value = (char*)malloc(1);
            *value = '\0';
        }

        SString(const char* charString) {
            if (charString != nullptr && strlen(charString) < 300)
            {
                value = (char*)malloc(strlen(charString) + 1);
                strcpy(value, charString);
            }
            else
            {
                value = (char*)malloc(1);
                *value = '\0';
            }
        }

        SString(const SString& other) {
            value = (char*)malloc(strlen(other.value) + 1);
            strcpy(value, other.value);
        }

        SString& operator= (const SString & other) {
            free(value);
            value = (char*)malloc(strlen(other.value) + 1);
            strcpy(value, other.value);
            return *this;
        }

        SString& operator+= (const SString & other)
        {
            char* newValue = (char*)malloc(strlen(value) + strlen(other.value) + 1);
            strcpy(newValue, value);
            strcpy(newValue + strlen(newValue), other.value);
            free(value);
            value = newValue;

            return *this;
        }

        SString operator+ (const SString & other)
        {
            SString str;

            str += *this;
            str += other;

            return str;
        }

        char* GetText() {
            return value;
        }

        void Clear()
        {
            free(value);
            value = (char*)malloc(1);
            *value = '\0';
        }

        ~SString() {
            free(value);
            value = nullptr;
        }

        friend std::ostream & operator << (std::ostream &out, const SString &c);
    };

    inline
    std::ostream & operator << (std::ostream &out, const Types::SString &c)
    {
        out << c.value;
        return out;
    }
}

