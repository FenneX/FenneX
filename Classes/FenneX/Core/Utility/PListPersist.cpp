/****************************************************************************
Copyright (c) 2013-2014 Auticiel SAS

http://www.fennex.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************///

#include "PListPersist.h"
#include "Shorteners.h"
#include <errno.h>
#include <unistd.h>
#include "NativeUtility.h"
#include "AppMacros.h"
#include "FenneXMacros.h"

using namespace pugi;

NS_FENNEX_BEGIN
//Code from code_save_custom_writer to print doc result
struct xml_string_writer: pugi::xml_writer
{
    std::string result;
    
    virtual void write(const void* data, size_t size)
    {
        result += std::string(static_cast<const char*>(data), size);
    }
};

std::string node_to_string(pugi::xml_node node)
{
    xml_string_writer writer;
    node.print(writer);
    
    return writer.result;
}
void appendObject(Value& val, xml_node& node)
{
    xml_node children;
    switch(val.getType())
    {
        case Value::Type::MAP:
            children = node.append_child("dict");
            for(ValueMap::iterator it = val.asValueMap().begin(); it != val.asValueMap().end(); ++it) {
                children.append_child("key").append_child(node_pcdata).set_value(it->first.c_str());
                appendObject(it->second, children);
            }
            break;
        case Value::Type::INT_KEY_MAP:
            children = node.append_child("intKeydict");
            for(ValueMapIntKey::iterator it = val.asIntKeyMap().begin(); it != val.asIntKeyMap().end(); ++it)
            {
                children.append_child("key").append_child(node_pcdata).set_value(std::to_string(it->first).c_str());
                appendObject(it->second, children);
            }
            break;
        case Value::Type::VECTOR:
            children = node.append_child("array");
            for(ValueVector::iterator it = val.asValueVector().begin(); it < val.asValueVector().end(); it++) {
                appendObject(*it, children);
            }
            break;
        case Value::Type::STRING:
            node.append_child("string").append_child(node_pcdata).set_value(val.asString().c_str());
            break;
        case Value::Type::INTEGER:
            node.append_child("integer").append_child(node_pcdata).set_value(val.asString().c_str());
            break;
        case Value::Type::FLOAT:
        case Value::Type::DOUBLE:
            node.append_child("real").append_child(node_pcdata).set_value(val.asString().c_str());
            break;
        case Value::Type::BOOLEAN:
            node.append_child(val.asBool() ? "true" : "false");
            break;
        default:
#if VERBOSE_SAVE_PLIST
            log("Warning: unrecognized Value type when saving plist, check if the object is in plist format. Value description: %s", val.getDescription().c_str());
#endif
            break;
    }
}

void saveValueToFile(Value& val, std::string fileName, FileLocation location)
{
    CCAssert(location != FileLocation::Resources, "Cannot save file to resources, it is read-only");
    xml_document doc;
    //add the verbose things so that it's a proper plist like those created by xcode
    xml_node decl = doc.prepend_child(node_declaration);
    decl.append_attribute("version") = "1.0";
    decl.append_attribute("encoding") = "UTF-8";
    xml_node doctype = doc.append_child(node_doctype);
    doctype.set_value("plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
    xml_node plistNode = doc.append_child("plist");

    //construct the actual plist informations
    appendObject(val, plistNode);
    std::string fullPath = getFullPath(fileName, location);
#if VERBOSE_SAVE_PLIST
    log("Saving document %s :\n%s", fileName.c_str(), node_to_string(doc).c_str());
    log("Saving to full %s path %s", location == FileLocation::Public ? "external" : "local", fullPath.c_str());
#endif
    doc.save_file(fullPath.c_str());
#if VERBOSE_SAVE_PLIST
    log("Document saved!");
#endif
}

Value loadValue(xml_node node)
{
    const char* name = node.name();
    Value val;
    if(strcmp(name, "dict") == 0)
    {
        ValueMap map;
        char* key;
        bool isKey = true;
        for(xml_node child = node.first_child(); child; child = child.next_sibling())
        {
            if(isKey)
            {
                key = const_cast<char*>(child.first_child().value());//remove const while reading value, easier that way
            }
            else
            {
                Value result = loadValue(child);
                if(!result.isNull())
                {
                    map[key] = result;
                }
            }
            isKey = !isKey;
        }
        val = map;
    }
    else if(strcmp(name, "intKeydict") == 0)
    {
        ValueMapIntKey map;
        char* key;
        bool isKey = true;
        for(xml_node child = node.first_child(); child; child = child.next_sibling())
        {
            if(isKey)
            {
                key = const_cast<char*>(child.first_child().value());//remove const while reading value, easier that way
            }
            else
            {
                Value result = loadValue(child);
                if(!result.isNull())
                {
                    map[std::atoi(key)] = result;
                }
            }
            isKey = !isKey;
        }
        val = map;
    }
    else if(strcmp(name, "intKeydict") == 0)
    {
        ValueMapIntKey map;
        char* key;
        bool isKey = true;
        for(xml_node child = node.first_child(); child; child = child.next_sibling())
        {
            if(isKey)
            {
                key = const_cast<char*>(child.first_child().value());//remove const while reading value, easier that way
            }
            else
            {
                Value result = loadValue(child);
                if(result.getType() != Value::Type::NONE)
                {
                    map[std::atoi(key)] = result;
                }
            }
            isKey = !isKey;
        }
        val = map;
    }
    else if(strcmp(name, "array") == 0)
    {
        ValueVector vector;
        for(xml_node child = node.first_child(); child; child = child.next_sibling())
        {
            Value result = loadValue(child);
            if(!result.isNull())
            {
                vector.push_back(result);
            }
        }
        val = vector;
    }
    else if(strcmp(name, "string") == 0)
    {
        val = Value(node.first_child().value());
    }
    else if(strcmp(name, "integer") == 0)
    {
        val = Value(atoi(node.first_child().value()));
    }
    else if(strcmp(name, "real") == 0)
    {
        val = Value(atof(node.first_child().value()));
    }
    else if(strcmp(name, "true") == 0)
    {
        val = Value(true);
    }
    else if(strcmp(name, "false") == 0)
    {
        val = Value(false);
    }
#if VERBOSE_LOAD_PLIST
    else
    {
        log("Warning: unrecognized type \"%s\" when loading plist, check if the plist is correctly formated", name);
    }
#endif
    return val;
}

Value loadValueFromFile(std::string fileName, FileLocation location)
{
    xml_document doc;
#if VERBOSE_LOAD_PLIST
    log("local path : %s", getLocalPath(fileName).c_str());
#endif
    std::string charbuffer = "";
    std::string path = getFullPath(fileName, location);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    if(location == FileLocation::Resources)
    {
        ssize_t bufferSize = 0;
        //Load file from apk
        charbuffer = FileUtils::getInstance()->getStringFromFile(fileName);
    }
#endif
#if VERBOSE_LOAD_PLIST
    log("Loading from path :\n%s", path.c_str());
#endif
    xml_parse_result parse_result;
    //If the file inside the apk doesn't exist, we load the local file.
    if(charbuffer.empty())
        parse_result = doc.load_file(path.c_str());
    else
        parse_result = doc.load(charbuffer.c_str());
#if VERBOSE_LOAD_PLIST
    log("parse result : %d", parse_result.status);
#endif
    if(parse_result.status != status_ok)
    {
        Value emptyVal;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        if(location == FileLocation::Resources)
        {
            log("Copying resource file to local ...");
            copyResourceFileToLocal(fileName);
        }
        parse_result = doc.load_file(getLocalPath(fileName).c_str());
#if VERBOSE_LOAD_PLIST
        log("parse result after copy : %d", parse_result.status);
#endif

        if(parse_result.status != status_ok)
        {
            return emptyVal;
        }
#else
        return emptyVal;
#endif
    }
#if VERBOSE_LOAD_PLIST
    log("Document loaded, parsing it");
    log("%s", node_to_string(doc).c_str()); //can't be done because this crash on Android, probably due to String_en.plist containing % not escaped properly
#endif
    Value result = loadValue(doc.child("plist").first_child());

#if VERBOSE_LOAD_PLIST
    log("Parse successful, returning");
#endif
    return result;
}
NS_FENNEX_END
