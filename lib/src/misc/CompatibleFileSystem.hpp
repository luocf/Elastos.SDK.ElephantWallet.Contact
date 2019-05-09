/**
 * @file	ThreadPool.hpp
 * @brief	ThreadPool
 * @details	
 *
 * @author	xxx
 * @author	<xxx@xxx.com>
 * @copyright	(c) 2012 xxx All rights reserved.
 **/

#ifndef _ELASTOS_FILE_SYSTEM_HPP_
#define _ELASTOS_FILE_SYSTEM_HPP_

#ifndef __APPLE__
#include <filesystem>
#else /* __APPLE__ */
#include "ghc-filesystem.hpp"
namespace std {
namespace filesystem = ghc::filesystem;
}
#endif /* __APPLE__ */


#endif /* _ELASTOS_FILE_SYSTEM_HPP_ */
