reflectClass
============
This file is used to implement the reflection with c++;

it's very simple, and support c++0x well(acturly, there only a little c++11 features...).

1. How to use it?
   Step0: include the header file class_factory.h
   Step1: Declear your class or interface in some header file(etc ./test/class_define.h), you can:
          Declear the interface class with macro "DECLEAR_INTERFACE"                                                 or
          Declear the singleton class with macro "DECLEAR_SINGLETON_CLASS"                                           or
          Declear the class which is inherited from the base_class with macro "DECLEAR_CLASS_EX"                     or
          Declear the singleton class which is inherited from the base_class with macro "DECLEAR_SINGLETON_CLASS_EX" or
          Declear the ordinery class with macro "DECLEAR_CLASS"
   Step2: Define the class you decleard in step1 in some source files(etc ./test/class_define.cpp), only 
          using the macro DEFINE_CLASS
   Step3: you can get the pointer of the class with (class_name*)ClassFactory::GetClass(class_name);

2. example:
   $make
   $./reflect
   
3. Bugs and grammar mistakes can be report to yaolu1103@gmail.com

4. License 
Copyright (c) 2014 Yao Lu All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the names of their
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
