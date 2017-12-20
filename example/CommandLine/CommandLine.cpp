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

#include <exception>
#include <iostream>
#include <vector>

#include "Cats/Corecat/String.hpp"
#include "Cats/Corecat/Util/CommandLine.hpp"


using namespace Cats::Corecat;
using namespace Cats::Corecat::Util::CommandLine;


template <typename T>
std::ostream& operator <<(std::ostream& a, const std::vector<T>& b) {
    
    a << '{';
    bool first = true;
    for(auto&& x : b) {
        
        if(first) first = false;
        else a << ", ";
        a << x;
        
    }
    a << '}';
    return a;
    
}


struct Options {
    
    bool help = false;
    bool version = false;
    String8 optimize;
    String8 output;
    std::vector<String8> input;
    
};

int main(int argc, char** argv) {
    
    try {
        
        Options options;
        CommandLineParser parser;
        CommandLineOptionList optionList;
        CommandLineLongOptionParser longOption;
        CommandLineShortOptionParser shortOption;
        CommandLineArgumentOptionParser argument;
        optionList.add(options.help).set(shortOption, "h").set(longOption, "help")
            .setHelp("Display help information");
        optionList.add(options.version).set(shortOption, "v").set(longOption, "version")
            .setHelp("Display version information");
        optionList.add(options.optimize).set(shortOption, "O")
            .setArgument("level").setHelp("Set optimization level to <level>");
        optionList.add(options.output).set(shortOption, "o").set(longOption, "output")
            .setArgument("file").setHelp("Output to <file>");
        optionList.add(options.input).set(argument);
        parser.add(optionList);
        parser.add(longOption).add(shortOption).add(argument);
        parser.parse(argc, argv);
        
        if(options.help) {
            
            std::cout << "Usage: CommandLine [options] [file]..." << std::endl;
            std::cout << parser.getHelp() << std::endl;
            
        } else if(options.version) {
            
            std::cout << "CommandLine 1.0.0" << std::endl;
            
        } else {
            
            std::cout << "Input files: " << options.input << std::endl;
            std::cout << "Output files: " << options.output << std::endl;
            std::cout << "Optimization level: " << options.optimize << std::endl;
            
        }
        
    } catch(std::exception& e) { std::cout << e.what() << std::endl; }
    
    return 0;
    
}
