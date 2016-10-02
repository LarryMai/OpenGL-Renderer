/**
 *  @file oglplus/config/site.hpp
 *  @brief System configuration options
 *
 *  @author Matus Chochlik
 *
 *  Copyright 2010-2015 Matus Chochlik. Distributed under the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef OGLPLUS_SITE_CONFIG_120305_HPP
#define OGLPLUS_SITE_CONFIG_120305_HPP

#define OGLPLUS_OPENAL_FOUND 0
#define OGLPLUS_FREEGLUT_FOUND 
#define OGLPLUS_PNG_FOUND 0
#define OGLPLUS_PANGO_CAIRO_FOUND 0

#define OGLPLUS_GL_VERSION_MAJOR 4
#define OGLPLUS_GL_VERSION_MINOR 3

#ifndef OGLPLUS_LOW_PROFILE
#if 0
#define OGLPLUS_LOW_PROFILE 0
#endif
#endif

#ifndef OGLPLUS_USE_GLCOREARB_H
#define OGLPLUS_USE_GLCOREARB_H 0
#endif

#ifndef OGLPLUS_USE_GL3_H
#define OGLPLUS_USE_GL3_H 0
#endif

#ifndef OGLPLUS_USE_GLEW
#define OGLPLUS_USE_GLEW 1
#endif

#ifndef OGLPLUS_USE_GL3W
#define OGLPLUS_USE_GL3W 0
#endif

#ifndef OGLPLUS_USE_BOOST_CONFIG
#define OGLPLUS_USE_BOOST_CONFIG 0
#endif

#if !OGLPLUS_USE_BOOST_CONFIG
#ifndef OGLPLUS_NO_SCOPED_ENUMS
#define OGLPLUS_NO_SCOPED_ENUMS 0
#endif

#ifndef OGLPLUS_NO_VARIADIC_MACROS
#define OGLPLUS_NO_VARIADIC_MACROS 0
#endif

#ifndef OGLPLUS_NO_VARIADIC_TEMPLATES
#define OGLPLUS_NO_VARIADIC_TEMPLATES 1
#endif

#ifndef OGLPLUS_NO_UNIFIED_INITIALIZATION_SYNTAX
#define OGLPLUS_NO_UNIFIED_INITIALIZATION_SYNTAX 0
#endif

#ifndef OGLPLUS_NO_INITIALIZER_LISTS
#define OGLPLUS_NO_INITIALIZER_LISTS 0
#endif

#ifndef OGLPLUS_NO_DEFAULTED_FUNCTIONS
#define OGLPLUS_NO_DEFAULTED_FUNCTIONS 0
#endif

#ifndef OGLPLUS_NO_DELETED_FUNCTIONS
#define OGLPLUS_NO_DELETED_FUNCTIONS 0
#endif

#ifndef OGLPLUS_NO_EXPLICIT_CONVERSION_OPERATORS
#define OGLPLUS_NO_EXPLICIT_CONVERSION_OPERATORS 0
#endif

#ifndef OGLPLUS_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define OGLPLUS_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS 0
#endif

#ifndef OGLPLUS_NO_UNICODE_LITERALS
#define OGLPLUS_NO_UNICODE_LITERALS 0
#endif

#ifndef OGLPLUS_NO_USER_DEFINED_LITERALS
#define OGLPLUS_NO_USER_DEFINED_LITERALS 0
#endif

#ifndef OGLPLUS_NO_TEMPLATE_ALIASES
#define OGLPLUS_NO_TEMPLATE_ALIASES 0
#endif

#ifndef OGLPLUS_NO_CONSTEXPR
#define OGLPLUS_NO_CONSTEXPR 0
#endif

#ifndef OGLPLUS_NO_OVERRIDE
#define OGLPLUS_NO_OVERRIDE 0
#endif

#ifndef OGLPLUS_NO_NOEXCEPT
#define OGLPLUS_NO_NOEXCEPT 0
#endif

#ifndef OGLPLUS_NO_LAMBDAS
#define OGLPLUS_NO_LAMBDAS 0
#endif

#endif // !OGLPLUS_USE_BOOST_CONFIG

#ifndef OGLPLUS_NO_SCOPED_ENUM_TEMPLATE_PARAMS
#define OGLPLUS_NO_SCOPED_ENUM_TEMPLATE_PARAMS 0
#endif

#ifndef OGLPLUS_NO_INHERITED_CONSTRUCTORS
#define OGLPLUS_NO_INHERITED_CONSTRUCTORS 0
#endif

#ifndef OGLPLUS_NO_GENERALIZED_ATTRIBUTES
#define OGLPLUS_NO_GENERALIZED_ATTRIBUTES 0
#endif

#ifndef OGLPLUS_NO_CHRONO
#define OGLPLUS_NO_CHRONO 0
#endif

#ifndef OGLPLUS_NO_THREADS
#define OGLPLUS_NO_THREADS 0
#endif

#endif
