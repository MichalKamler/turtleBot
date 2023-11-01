/**
 * @file /include/ecl/ipc/semaphore_pos.hpp
 *
 * @brief The posix implementation for semaphores.
 *
 * @date August 2009
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef ECL_IPC_SEMAPHORE_POS_HPP_
#define ECL_IPC_SEMAPHORE_POS_HPP_

/*****************************************************************************
** Platform Check
*****************************************************************************/

#include <ecl/config.hpp>
#if defined(ECL_IS_POSIX)
#include <unistd.h>
#ifdef _POSIX_SEMAPHORES
#if _POSIX_SEMAPHORES > 0

/*****************************************************************************
** Ecl Functionality Defines
*****************************************************************************/

#ifndef ECL_HAS_POSIX_SEMAPHORES
  #define ECL_HAS_POSIX_SEMAPHORES
#endif
#ifndef ECL_HAS_SEMAPHORES
  #define ECL_HAS_SEMAPHORES
#endif

/*****************************************************************************
** Includes
*****************************************************************************/

#include <iostream>
#include <string>
#include <semaphore.h>
#include <fcntl.h>           /* For O_* constants */
#include <ecl/exceptions/standard_exception.hpp>
#include <ecl/time/duration.hpp>

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace ecl {

/*****************************************************************************
** Forward Declarations
*****************************************************************************/

class Semaphore;

#ifdef ECL_HAS_EXCEPTIONS

namespace ipc {

/*****************************************************************************
** Semaphore Exceptions
*****************************************************************************/
/**
 * This static function generates a custom StandardException response
 * for posix error numbers generated by sem_open calls within
 * the Semaphore class. Relevant man page is:
 *
 * - sem_open
 *
 * @param loc : use with the LOC macro, identifies the line and file of the code.
 */
ECL_LOCAL ecl::StandardException openSemaphoreException(const char* loc );
/**
 * This static function generates a custom StandardException response
 * for posix error numbers generated by sem_trywait calls within
 * the Semaphore class. Relevant man page is:
 *
 * - sem_trywait
 *
 * @param loc : use with the LOC macro, identifies the line and file of the code.
 */
ECL_LOCAL ecl::StandardException tryLockSemaphoreException(const char* loc);

} // namespace ipc

#endif /* ECL_HAS_EXCEPTIONS */

/*****************************************************************************
** Semaphores
*****************************************************************************/
/**
 * @brief Inter-process locking mechanism for securing shared data.
 *
 * Secures data stored in shared memory via a mutual exclusion style
 * construction. There are other uses for semaphores, and alot more
 * complexity that can be taken advantage of. However this class' intention
 * remains simple, if such complexity is needed, other semaphore wrappers
 * will be created.
 *
 * Note: windows calls this a Mutex
 * which is slightly confusing - the windoze mutex is simply a binary
 * version of the posix semaphore).
 *
 * Usage should be easy to ascertain after glancing at the Semaphore
 * interface.
 **/
class ECL_PUBLIC Semaphore
{
public:
	/*********************
	** C&D's
	**********************/
	/**
	 * @brief Default constructor (always throws).
	 *
	 * This is a pure RAII style object and subsequently will always throw.
	 *
	 * @exception StandardException : automatically throws if used.
	 */
	Semaphore() throw(StandardException);
	/**
	 * @brief Initialises a connection to a semaphore.
	 *
	 * RAII style constructor that creates (if not already existing)
	 * and opens a connection to a semaphore with
	 * the specified string identifier (do not use any '/'s or special characters).
	 * Note that this automatically prepends a '/' to create a posix semaphore name.
	 *
	 * @param string_id : unique string identifier for the shared memory (no slashes)
	 *
	 * @exception StandardException : throws if the semaphore didn't initialise properly [debug mode only].
	 */
	Semaphore(const std::string& string_id) ecl_assert_throw_decl(StandardException);
	/**
	 * @brief Clean up the semaphore handles.
	 *
	 * Cleans up the posix structures associated with the semaphore.
	 **/
	virtual ~Semaphore();

	/*********************
	** Locking
	**********************/
	/**
	 * @brief Lock the semaphore.
	 *
	 * Lock access to the semaphore. If already locked, wait for it to unlock before locking.
	 **/
	void lock();
	/**
	 * @brief Unlock the semaphore.
	 *
	 * Unlock the semaphore.
	 **/
	void unlock();
	/**
	 * @brief Only lock the semaphore if currently unlocked.
	 *
	 * This attempts to lock the semaphore, but will not wait around if it is already locked.
	 * @return bool : success or failure to lock the semaphore.
	 */
	bool trylock();
	/**
	 * @brief Only lock the semaphore if it can before the timeout occurs.
	 *
	 * Try locking the semaphore over the specified duration.
	 * This duration is a relative time period (starting from
	 * the instant this function is called).
	 *
	 * Note: this function is not always available on posix systems (e.g. macosx).
	 * When it is not present, it will simply default its behaviour to that of a
	 * regular trylock().
	 *
	 * @param timeout : stop trying to lock if the timeout is reached.
	 *
	 * @exception StandardException : throws if locking attempt aborted (other than timeouts) [debug mode only].
	 **/
	bool trylock( const Duration &timeout ) ecl_debug_throw_decl(StandardException);

private:
	std::string name;
	sem_t* semaphore;

	/**
	 * @brief Return the semaphore count.
	 *
	 * This is not transparent to the user of the class as the semaphore here
	 * functions simply as a binary lock to shared memory. It is used however internally.
	 **/
	int count();
};


} // namespace ecl


#endif /* _POSIX_SEMAPHORES > 0 */
#endif /* _POSIX_SEMAPHORES */
#endif /* ECL_IS_POSIX */

#endif /* ECL_IPC_SEMAPHORE_POS_HPP_ */