/*
 *
 * MIT License
 *
 * Copyright (c) 2016-2017 The Cats Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef CATS_CORECAT_UTIL_COMMANDLINE_HPP
#define CATS_CORECAT_UTIL_COMMANDLINE_HPP


#include <cstddef>

#include <deque>
#include <functional>
#include <memory>
#include <vector>

#include "ArrayView.hpp"
#include "Exception.hpp"
#include "../Text/String.hpp"


namespace Cats {
namespace Corecat {
namespace Util {

class CommandLineOptionParser;

class CommandLineParseException : public Exception {
    
private:
    
    using String8 = Text::String8;
    
public:
    
    CommandLineParseException(const String8& data) : Exception("CommandLineParseException: " + data) {}
    
};

class CommandLineOption {
    
private:
    
    using String8 = Text::String8;
    using StringView8 = Text::StringView8;
    
private:
    
    std::function<void(StringView8)> callback;
    bool required = false;
    std::vector<std::pair<CommandLineOptionParser*, String8>> parsers;
    String8 argument;
    String8 help;
    
public:
    
    CommandLineOption(std::function<void(StringView8)> callback_) : callback(std::move(callback_)) {}
    CommandLineOption(const CommandLineOption& src) = delete;
    CommandLineOption(CommandLineOption&& src) = default;
    
    CommandLineOption& operator =(const CommandLineOption& src) = delete;
    CommandLineOption& operator =(CommandLineOption&& src) = default;
    
    template <typename T, typename... Arg>
    CommandLineOption& set(T&& parser, Arg&&... arg) { parser.set(*this, std::forward<Arg>(arg)...); return *this; }
    
    const std::function<void(StringView8)>& getCallback() const noexcept { return callback; }
    CommandLineOption& setCallback(std::function<void(StringView8)> callback_) noexcept { callback = std::move(callback_); return *this; }
    bool getRequired() const noexcept { return required; }
    CommandLineOption& setRequired(bool required_ = true) noexcept { required = required_; return *this; }
    const std::vector<std::pair<CommandLineOptionParser*, String8>>& getParsers() const noexcept { return parsers; }
    CommandLineOption& addParser(CommandLineOptionParser& parser, String8 name) { parsers.emplace_back(&parser, std::move(name)); return *this; }
    const String8& getArgument() const noexcept { return argument; }
    CommandLineOption& setArgument(String8 argument_) { argument = std::move(argument_); return *this; }
    const String8& getHelp() const noexcept { return help; }
    CommandLineOption& setHelp(String8 help_) { help = std::move(help_); return *this; }
    
};


class CommandLineOptionParser {
    
private:
    
    using String8 = Text::String8;
    using StringView8 = Text::StringView8;
    
public:
    
    virtual ~CommandLineOptionParser() = default;
    
    virtual String8 getName(const CommandLineOption& option, const String8& name) const = 0;
    virtual std::size_t parse(ArrayView<const StringView8> arg) const = 0;
    
};


class CommandLineLongOptionParser : public CommandLineOptionParser {
    
private:
    
    using String8 = Text::String8;
    using StringView8 = Text::StringView8;
    
private:
    
    std::vector<std::pair<String8, CommandLineOption*>> options;
    
public:
    
    CommandLineLongOptionParser() = default;
    CommandLineLongOptionParser(const CommandLineLongOptionParser& src) = delete;
    
    CommandLineLongOptionParser& operator =(const CommandLineLongOptionParser& src) = delete;
    
    void set(CommandLineOption& option, String8 name) {
        
        option.addParser(*this, name);
        options.emplace_back(std::move(name), &option);
        
    }
    
    String8 getName(const CommandLineOption& option, const String8& name) const override {
        
        if(option.getRequired()) return "--" + name + "=<" + option.getArgument() + '>';
        else return "--" + name;
        
    }
    std::size_t parse(ArrayView<const StringView8> arg) const override {
        
        if(arg.isEmpty()) return 0;
        if(!arg[0].startsWith("--")) return 0;
        for(auto&& x : options) {
            
            auto& name = x.first;
            auto& option = *x.second;
            if(!arg[0].slice(2).startsWith(name)) continue;
            auto argument = arg[0].slice(2 + name.getLength());
            if(!argument.isEmpty() && !argument.startsWith("=")) continue;
            auto& callback = option.getCallback();
            if(option.getRequired()) {
                
                if(argument.isEmpty()) {
                    
                    if(arg.getSize() == 1) throw CommandLineParseException("missing argument after --" + name);
                    callback(arg[1]);
                    return 2;
                    
                } else {
                    
                    callback(argument.slice(1));
                    return 1;
                    
                }
                
            } else {
                
                if(!argument.isEmpty()) throw CommandLineParseException("unexcepted argument after --" + name);
                callback({});
                return 1;
                
            }
            
        }
        throw CommandLineParseException("unexcepted option " + arg[0]);
        
    }
    
};

class CommandLineShortOptionParser : public CommandLineOptionParser {
    
private:
    
    using String8 = Text::String8;
    using StringView8 = Text::StringView8;
    
private:
    
    std::vector<std::pair<String8, CommandLineOption*>> options;
    
public:
    
    CommandLineShortOptionParser() = default;
    CommandLineShortOptionParser(const CommandLineShortOptionParser& src) = delete;
    
    CommandLineShortOptionParser& operator =(const CommandLineShortOptionParser& src) = delete;
    
    void set(CommandLineOption& option, String8 name) {
        
        option.addParser(*this, name);
        options.emplace_back(std::move(name), &option);
        
    }
    
    String8 getName(const CommandLineOption& option, const String8& name) const override {
        
        if(option.getRequired()) return '-' + name + " <" + option.getArgument() + '>';
        else return '-' + name;
        
    }
    std::size_t parse(ArrayView<const StringView8> arg) const override {
        
        if(arg.isEmpty()) return 0;
        if(!arg[0].startsWith("-")) return 0;
        for(auto&& x : options) {
            
            auto& name = x.first;
            auto& option = *x.second;
            if(!arg[0].slice(1).startsWith(name)) continue;
            auto argument = arg[0].slice(1 + name.getLength());
            auto& callback = option.getCallback();
            if(option.getRequired()) {
                
                if(argument.isEmpty()) {
                    
                    if(arg.getSize() == 1) throw CommandLineParseException("missing argument after -" + name);
                    callback(arg[1]);
                    return 2;
                    
                } else {
                    
                    callback(argument);
                    return 1;
                    
                }
                
            } else {
                
                if(!argument.isEmpty()) throw CommandLineParseException("unexcepted argument after -" + name);
                callback({});
                return 1;
                
            }
            
        }
        throw CommandLineParseException("unexcepted option " + arg[0]);
        
    }
    
};

class CommandLineArgumentOptionParser : public CommandLineOptionParser {
    
private:
    
    using String8 = Text::String8;
    using StringView8 = Text::StringView8;
    
private:
    
    std::vector<CommandLineOption*> options;
    
public:
    
    CommandLineArgumentOptionParser() = default;
    CommandLineArgumentOptionParser(const CommandLineArgumentOptionParser& src) = delete;
    
    CommandLineArgumentOptionParser& operator =(const CommandLineArgumentOptionParser& src) = delete;
    
    void set(CommandLineOption& option) {
        
        option.addParser(*this, {});
        options.emplace_back(&option);
        
    }
    
    String8 getName(const CommandLineOption& /*option*/, const String8& /*name*/) const override { return {}; }
    std::size_t parse(ArrayView<const StringView8> arg) const override {
        
        if(arg.isEmpty()) return 0;
        for(auto&& x : options) {
            
            auto& callback = x->getCallback();
            callback(arg[0]);
            
        }
        return 1;
        
    }
    
};


class CommandLineOptionList {
    
private:
    
    using String8 = Text::String8;
    using StringView8 = Text::StringView8;
    
private:
    
    String8 name;
    std::deque<CommandLineOption> options;
    
public:
    
    CommandLineOptionList(String8 name_ = "Options") : name(std::move(name_)) {}
    CommandLineOptionList(const CommandLineOptionList& src) = delete;
    CommandLineOptionList(CommandLineOptionList&& src) = default;
    
    CommandLineOptionList& operator =(const CommandLineOptionList& src) = delete;
    CommandLineOptionList& operator =(CommandLineOptionList&& src) = default;
    
    template <typename T>
    CommandLineOption& add(T&& t) { return *this << std::forward<T>(t); }
    
    const String8& getName() const { return name; }
    void setName(String8 name_) { name = std::move(name_); }
    const std::deque<CommandLineOption>& getOptions() const { return options; }
    CommandLineOption& addOption(std::function<void(StringView8)> callback) {
        
        options.emplace_back(std::move(callback));
        return options.back();
        
    }
    
};

inline CommandLineOption& operator <<(CommandLineOptionList& optionList, std::function<void(Text::StringView8)> callback) {
    
    return optionList.addOption(std::move(callback));
    
}
inline CommandLineOption& operator <<(CommandLineOptionList& optionList, bool& x) {
    
    return optionList << [&x](Text::StringView8 /*str*/) { x = true; };
    
}
inline CommandLineOption& operator <<(CommandLineOptionList& optionList, Text::String8& x) {
    
    return (optionList << [&x](Text::StringView8 str) { x = str; }).setRequired();
    
}
inline CommandLineOption& operator <<(CommandLineOptionList& optionList, std::vector<Text::String8>& x) {
    
    return (optionList << [&x](Text::StringView8 str) { x.emplace_back(str); }).setRequired();
    
}


class CommandLineParser {
    
private:
    
    using String8 = Text::String8;
    using StringView8 = Text::StringView8;
    
private:
    
    std::vector<const CommandLineOptionList*> optionLists;
    std::vector<const CommandLineOptionParser*> parsers;
    
public:
    
    CommandLineParser() = default;
    
    CommandLineParser& add(const CommandLineOptionList& optionList) { optionLists.emplace_back(&optionList); return *this; }
    CommandLineParser& add(const CommandLineOptionParser& parser) { parsers.emplace_back(&parser); return *this; }
    
    void parse(const std::vector<StringView8>& arg) const {
        
        ArrayView<const StringView8> a(arg.data(), arg.size());
        while(!a.isEmpty()) {
            
            bool matched = false;
            for(auto&& x : parsers) {
                
                std::size_t ret = x->parse(a);
                if(ret) {
                    
                    a = {a.getData() + ret, a.getSize() - ret};
                    matched = true;
                    break;
                    
                }
                
            }
            if(!matched) throw CommandLineParseException("unexcepted argument " + a[0]);
            
        }
        
    }
    void parse(int argc, char** argv) const { parse({argv + 1, argv + argc}); }
    
    String8 getHelp() const {
        
        String8 helpText;
        for(auto&& optionList : optionLists) {
            
            helpText += optionList->getName() + ":\n";
            for(auto&& option : optionList->getOptions()) {
                
                if(option.getHelp().isEmpty()) continue;
                String8 optionText;
                bool first = true;
                for(auto&& z : option.getParsers()) {
                    
                    auto& parser = *z.first;
                    auto& name = z.second;
                    auto text = parser.getName(option, name);
                    if(!text.isEmpty()) {
                        
                        if(first) first = false;
                        else optionText += ", ";
                        optionText += text;
                        
                    }
                    
                }
                if(optionText.isEmpty()) continue;
                helpText += "    " + optionText;
                if(optionText.getLength() <= 26)
                    helpText += String8(' ', 28 - optionText.getLength());
                else
                    helpText += '\n' + String8(' ', 32);
                helpText += option.getHelp() + '\n';
                
            }
            
        }
        return helpText;
        
    }
    
};

}
}
}


#endif
