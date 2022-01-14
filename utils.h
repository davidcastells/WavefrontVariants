/**
 * 
 * Copyright (C) 2021, David Castells-Rufas <david.castells@uab.cat>, 
 * Universitat Autonoma de Barcelona  
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#define min2(a,b) (((a)<(b))?(a):(b))
#define max2(a,b) (((a)>(b))?(a):(b))
#define min3(a,b,c) min2(a, min2(b, c))
#define max3(a,b,c) max2(a, max2(b, c))

#define TEXT_SCAPE_BOLD         "\e[1m"
#define TEXT_SCAPE_ITALIC       "\e[3m"
#define TEXT_SCAPE_UNDERLINE    "\e[4m"
#define TEXT_SCAPE_END          "\e[0m"

#endif