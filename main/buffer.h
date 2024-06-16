/*
   Copyright 2013-2023 Marco Bacchi <marco@bacchi.at>
   
   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use,
   copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following
   conditions:
   
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * @file
 * @copyright  Copyright 2013-2023 Marco Bacchi <marco@bacchi.at>
 * @license    This file is released under the MIT License.\n
 *             https://opensource.org/licenses/MIT
 *
 * @brief Generic Buffer Implementation
 *
 * @details This file contains an abstract and generic implementation of a buffer
 *          using preprocessor macros. The implementation is designed to be flexible
 *          and adaptable to various data types and buffer sizes. It includes macros
 *          for defining buffer types, initializing buffers, and performing common
 *          buffer operations such as adding items, checking fullness or emptiness,
 *          and accessing buffer elements.
 *
 * Buffers in General:
 *          A buffer is a contiguous block of memory used to store a collection of
 *          objects of a certain data type. Buffers are commonly used in various
 *          programming scenarios, such as IO operations, communication systems,
 *          data processing, and temporary storage of data. They are essential in
 *          scenarios where there is a difference in the rate at which data is
 *          received and processed, serving as a temporary holding place for the
 *          data during these operations.
 *
 *          Buffers can be implemented in various ways depending on the requirements,
 *          such as circular buffers for streaming data, FIFO (First-In-First-Out) for
 *          queue implementations, or simple static arrays for fixed-size storage.
 *
 *          This implementation provides a generic way to define and manipulate buffers
 *          using macros, making it adaptable to a wide range of use cases. It allows
 *          users to define buffers for different data types and sizes and includes
 *          necessary operations for managing buffer data effectively.
 *
 * Usage:
 *          To use this implementation, include this header file in your project, and
 *          utilize the provided macros to define and work with buffers. Ensure to check
 *          buffer constraints like fullness or emptiness before performing relevant
 *          operations.
 *
 *
 *          This file was documented in doxygen style format by using ChatGPT 4.
 *
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>
#include <stddef.h>



/**
 * @brief Declares a new buffer type.
 *
 * @details This macro is used to declare a new buffer type with a specified name, item type, and size.
 *          It creates a typedef for a struct, which includes a count of the current number of items in the
 *          buffer and an array of items of the specified type. The size of the array is set based on the
 *          provided size parameter. The defined type can be used in combination with `BUFFER_T` to declare
 *          buffer variables of this type, potentially with different storage class specifiers.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(intBuffer, int, 10);       // Creates a buffer type for integers with a capacity of 10
 * static BUFFER_T(intBuffer) myBuffer;      // Declares a static buffer variable of the newly created type
 * @endcode
 *
 * @param name The unique identification name for the buffer type. This name is used with `BUFFER_T` to define buffer variables.
 * @param type The data type of the items that the buffer will hold.
 * @param size The number of items of \a type that the buffer can hold.
 */
#define BUFFER_TYPEDEF(name,type,size)  \
    typedef struct {                    \
            size_t count;               \
            type items [size];          \
    } name##_buffer_t


/**
 * @brief Defines a buffer variable of a specific buffer type.
 *
 * @details This macro is used to define a buffer variable of the type created by `BUFFER_TYPEDEF`.
 *          The macro expands to the type name of the buffer, allowing for the declaration of buffer
 *          variables. It can be combined with different storage class specifiers (like static, extern, volatile)
 *          for additional behavior control of the buffer variable.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(intBuffer, int, 10);  // First, create a buffer type for integers
 * static BUFFER_T(intBuffer) myBuffer; // Then, define a static buffer variable of that type
 * @endcode
 *
 * @param name The unique identification name of the buffer type, which corresponds to the name used in `BUFFER_TYPEDEF`.
 */
#define BUFFER_T(name) \
    name##_buffer_t


/**
 * @brief Retrieves a pointer to the items array within a buffer.
 *
 * @details This macro provides access to the items contained in a buffer by returning
 *          a pointer to the first element of the buffer's internal array. The type of
 *          the returned pointer matches the type of the items in the buffer, as defined
 *          by the `BUFFER_TYPEDEF` macro. This allows for direct access and manipulation
 *          of the buffer's items.
 *
 *          It is useful for operations that require iterating over the buffer items or
 *          accessing them by index. Since the pointer is of the buffer's item type, it
 *          ensures type safety and consistency in buffer operations.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(intBuffer, int, 10);
 * BUFFER_T(intBuffer) myBuffer;
 * int* items = BUFFER_ITEMS(myBuffer);
 * items[0] = 5; // Directly access and modify the first item in the buffer
 * @endcode
 *
 * @param buffer The buffer variable from which to retrieve the items pointer.
 * @return A pointer of the buffer's item type, pointing to the buffer's internal items array.
 */
#define BUFFER_ITEMS(buffer) \
    ((buffer).items)

/**
 * @brief Returns a raw (untyped) pointer to the buffer items.
 * @details This macro provides access to the internal items of the buffer as
 *          a raw memory pointer. It casts the pointer to the buffer's items to
 *          a `uint8_t*` (pointer to unsigned 8-bit integers), enabling operations
 *          on the buffer's contents at the byte level. This can be useful for
 *          operations such as serialization, memory manipulation, or when interacting
 *          with APIs that require byte-level data access.
 *
 *          The returned pointer points to the start of the buffer's internal array,
 *          allowing direct access to its raw bytes. This should be used with caution,
 *          as it bypasses type safety provided by the buffer's defined item type.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * BUFFER_INIT(buffer);
 * uint8_t* rawItems = BUFFER_RAW(buffer);
 * // rawItems now points to the buffer's internal memory as a byte array
 * @endcode
 *
 * @param buffer The buffer instance from which to retrieve the raw items pointer.
 * @return A `uint8_t*` pointer to the buffer's internal items array.
 */
#define BUFFER_RAW(buffer) \
	((uint8_t*)BUFFER_ITEMS(buffer))

/**
 * @brief Returns the size of the buffer in bytes.
 * @details This macro calculates the total size of the buffer's items in bytes.
 *          It does this by taking the size of the buffer's internal array, which
 *          is determined using the `sizeof` operator. The result reflects the
 *          total memory footprint of the buffer's content, excluding any additional
 *          metadata like the item count. This size information can be particularly
 *          useful for operations like memory allocation, copying, or serialization
 *          where the byte-size of the buffer content is required.
 *
 *          It is important to note that this macro provides the size of the items
 *          array within the buffer, not the number of items currently stored in it.
 *          The size in bytes is constant, based on the buffer's declared capacity.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * size_t bufferSizeBytes = BUFFER_SIZEOF(buffer);
 * // bufferSizeBytes is now equal to the total byte size of 10 integers
 * @endcode
 *
 * @param buffer The buffer instance for which to determine the size in bytes.
 * @return The size of the buffer's items array in bytes.
 */
#define BUFFER_SIZEOF(buffer) \
    (sizeof(BUFFER_ITEMS(buffer)))


/**
 * @brief Returns the maximum capacity of the buffer in terms of number of items.
 * @details This macro calculates the maximum number of items that the buffer can hold.
 *          It divides the total size of the buffer's items array in bytes (obtained using
 *          `BUFFER_SIZEOF(buffer)`) by the size of a single item in the array. This
 *          calculation gives the total capacity of the buffer, indicating how many items
 *          of the specified type the buffer can store.
 *
 *          This macro is useful for understanding the limits of the buffer and ensuring
 *          that operations such as adding items do not exceed its capacity. It is
 *          particularly important in buffer management logic, where the buffer's capacity
 *          constraints must be respected to avoid overflows.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * size_t bufferCapacity = BUFFER_SIZE(buffer);
 * // bufferCapacity now holds the value 10, the maximum number of integers the buffer can store
 * @endcode
 *
 * @param buffer The buffer instance for which to determine the maximum capacity.
 * @return The maximum number of items that the buffer can hold.
 */
#define BUFFER_SIZE(buffer) \
    (BUFFER_SIZEOF(buffer)/sizeof(BUFFER_ITEMS(buffer)[0]))

/**
 * @brief Returns the current number of items in the buffer.
 * @details This macro retrieves the current count of items stored in the buffer.
 *          The count is maintained as part of the buffer's structure and is updated
 *          whenever items are added to or removed from the buffer. This macro
 *          provides a direct way to access this count, allowing for checks on how
 *          many items are currently stored in the buffer.
 *
 *          The count value is essential for operations such as adding new items
 *          (to prevent buffer overflow) or iterating over the items in the buffer.
 *          It provides a real-time count of how much of the buffer's capacity is being utilized.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * BUFFER_INIT(buffer);
 * // After adding items to the buffer...
 * size_t itemCount = BUFFER_COUNT(buffer);
 * // itemCount now holds the number of items currently in the buffer
 * @endcode
 *
 * @param buffer The buffer instance for which to determine the current item count.
 * @return The number of items currently stored in the buffer.
 */
#define BUFFER_COUNT(buffer) \
    ((buffer).count)

/**
 * @brief Initializes the buffer.
 * @details This macro initializes the specified buffer by setting its item count to zero.
 *          This operation effectively resets the buffer, marking it as empty and ready
 *          for use. It's a crucial step in buffer management, ensuring that the buffer
 *          starts in a known state before any items are added or removed.
 *
 *          Initialization should be performed immediately after declaring a buffer variable
 *          to avoid operating on uninitialized or potentially corrupted data. This macro
 *          simplifies the initialization process by encapsulating the necessary steps into
 *          a single, easy-to-use command.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * BUFFER_INIT(buffer);
 * // The buffer 'buffer' is now initialized and ready for use
 * @endcode
 *
 * @param buffer The buffer instance to be initialized.
 */
#define BUFFER_INIT(buffer) \
    do{BUFFER_COUNT(buffer)=0;}while(0)


/**
 * @brief Checks if the buffer is full.
 * @details This macro evaluates whether the buffer has reached its maximum capacity.
 *          It compares the current number of items in the buffer (obtained via
 *          `BUFFER_COUNT(buffer)`) with the buffer's maximum size (obtained via
 *          `BUFFER_SIZE(buffer)`). If the count of items is greater than or equal to
 *          the buffer's capacity, the macro returns 1, indicating that the buffer is full
 *          and cannot accommodate any more items. Otherwise, it returns 0.
 *
 *          This check is essential to prevent buffer overflows, especially before
 *          performing operations that add items to the buffer. Using this macro
 *          ensures that any such additions are made safely and within the buffer's capacity limits.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * BUFFER_INIT(buffer);
 * // After adding some items to the buffer...
 * if (BUFFER_FULL(buffer)) {
 *     // Buffer is full, take appropriate actions
 * } else {
 *     // Buffer still has space for more items
 * }
 * @endcode
 *
 * @param buffer The buffer instance to be checked for fullness.
 * @return 1 if the buffer is full, 0 otherwise.
 */
#define BUFFER_FULL(buffer) \
    (BUFFER_COUNT(buffer)>=BUFFER_SIZE(buffer))

/**
 * @brief Checks if the buffer is empty.
 * @details This macro determines whether the buffer currently contains no items.
 *          It evaluates the current item count in the buffer (using `BUFFER_COUNT(buffer)`)
 *          and returns 1 (true) if the count is zero, indicating that the buffer is empty.
 *          If the buffer contains one or more items, it returns 0 (false).
 *
 *          This check is useful for operations that require knowledge of whether the buffer
 *          has any data in it, such as avoiding underflows when attempting to remove or
 *          process items from the buffer. Using this macro ensures safe and efficient buffer
 *          management by providing a clear and concise check for the buffer's emptiness.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * BUFFER_INIT(buffer);
 * // After some operations on the buffer...
 * if (BUFFER_EMPTY(buffer)) {
 *     // Buffer is empty, no items to process
 * } else {
 *     // Buffer contains items that can be processed
 * }
 * @endcode
 *
 * @param buffer The buffer instance to check for emptiness.
 * @return 1 if the buffer is empty, 0 otherwise.
 */
#define BUFFER_EMPTY(buffer) \
    (!BUFFER_COUNT(buffer))


/**
 * @brief Advances the buffer to the next free item.
 * @details This macro increments the count of items in the buffer, effectively
 *          moving the buffer's position to the next free item. It is used after
 *          adding an item to the buffer to update the buffer's count accordingly.
 *
 *          It is important to note that this macro does not perform any check for
 *          buffer overflow. Therefore, its use without prior verification can lead
 *          to undefined behavior if the buffer is already full. It is recommended
 *          to always use `BUFFER_FULL` to check the buffer's capacity before
 *          adding new items and using this macro.
 *
 *          The macro simplifies the process of managing the buffer's count and
 *          ensures the buffer state is correctly updated after each insertion.
 *          However, caution must be exercised to avoid overflows.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * BUFFER_INIT(buffer);
 * if (!BUFFER_FULL(buffer)) {
 *     // Add item to buffer
 *     BUFFER_VAL(buffer) = newItem;
 *     BUFFER_NEXT(buffer); // Move to next free item
 * }
 * @endcode
 *
 * @param buffer The buffer instance to be advanced to the next item.
 */
#define BUFFER_NEXT(buffer) \
    (++BUFFER_COUNT(buffer))

/**
 * @brief Retrieves the current free item in the buffer.
 * @details This macro provides access to the next free (or current end) item in the
 *          buffer by returning its value. It does this by accessing the item in the
 *          buffer's internal array at the position indicated by the current item count.
 *          This macro is typically used to directly read or write the value of the next
 *          free item in the buffer.
 *
 *          It is important to ensure that the buffer is not full before using this macro
 *          to add new items. Writing to the buffer using this macro when it is full can
 *          lead to buffer overflow and undefined behavior. Always use `BUFFER_FULL`
 *          to check if there is space available in the buffer before using `BUFFER_VAL`.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * BUFFER_INIT(buffer);
 * if (!BUFFER_FULL(buffer)) {
 *     BUFFER_VAL(buffer) = newValue; // Set the value of the next free item
 *     BUFFER_NEXT(buffer);           // Advance the buffer to the next position
 * }
 * @endcode
 *
 * @param buffer The buffer instance from which to retrieve the current free item.
 * @return The value of the current free item in the buffer.
 */
#define BUFFER_VAL(buffer) \
    (BUFFER_ITEMS(buffer)[BUFFER_COUNT(buffer)])

/**
 * @brief Retrieves a pointer to the current free item in the buffer.
 * @details This macro provides a pointer to the next free (or current end) item in
 *          the buffer. It achieves this by obtaining the address of the item at the
 *          current buffer count position. This pointer can be used for direct manipulation
 *          or assignment of the buffer's next free item.
 *
 *          Similar to `BUFFER_VAL`, it's crucial to ensure that the buffer is not full
 *          before using this macro for adding new items. Writing to the buffer using
 *          this macro when the buffer is full can lead to buffer overflow and undefined
 *          behavior. Therefore, always check the buffer's capacity using `BUFFER_FULL`
 *          before using `BUFFER_PTR` to access or modify the buffer's contents.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * BUFFER_INIT(buffer);
 * if (!BUFFER_FULL(buffer)) {
 *     *BUFFER_PTR(buffer) = newValue; // Directly set the value of the next free item
 *     BUFFER_NEXT(buffer);            // Advance the buffer to the next position
 * }
 * @endcode
 *
 * @param buffer The buffer instance from which to retrieve the pointer to the current free item.
 * @return A pointer to the current free item in the buffer.
 */
#define BUFFER_PTR(buffer) \
    (&BUFFER_VAL(buffer))

/**
 * @brief Appends an item to the buffer.
 * @details This macro is used to append an item to the buffer by directly setting
 *          the value of the next free item and then incrementing the buffer's count.
 *          It simplifies the process of adding an item to the buffer, combining the
 *          assignment of the item's value and advancing the buffer count into a single step.
 *
 *          However, this macro does not perform a buffer overflow check. Therefore,
 *          it is important to use `BUFFER_FULL` to verify that there is space available
 *          in the buffer before using `BUFFER_APPEND`. Appending an item to a full buffer
 *          results in undefined behavior and could potentially lead to buffer overflows.
 *
 * Usage Example:
 * @code
 * BUFFER_TYPEDEF(myBuffer, int, 10);
 * BUFFER_T(myBuffer) buffer;
 * BUFFER_INIT(buffer);
 * if (!BUFFER_FULL(buffer)) {
 *     BUFFER_APPEND(buffer, newValue); // Append newValue to the buffer
 * }
 * @endcode
 *
 * @param buffer The buffer instance to which the item is appended.
 * @param item The item to append to the buffer.
 */
#define BUFFER_APPEND(buffer,item) \
	do{BUFFER_VAL(buffer) = item; BUFFER_NEXT(buffer);}while(0)


/**
 * @brief Iterates over each item in the buffer.
 *
 * This macro provides a way to iterate over each item in the buffer.
 * It sets up a loop structure, iterating through each item until it reaches
 * the end of the buffer.
 *
 * @param buffer The buffer to iterate over.
 * @param itemptr A pointer to the current item in the buffer.
 *
 * @note The buffer must be initialized before using this macro.
 * @note The macro creates a loop structure, so break and continue can be used as normal.
 */
#define BUFFER_FOREACH(buffer,itemptr) \
    for(itemptr=BUFFER_ITEMS(buffer); itemptr!=BUFFER_PTR(buffer); itemptr++)


#endif /* BUFFER_H_ */
