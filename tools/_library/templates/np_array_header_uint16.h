/*[[[cog
import cog

# Global constants expected
assert (file_prefix is not None)
assert (len(data_array) > 0)
assert (len(comment_strings) >= 0)

# Setting constants
uppercase_file_prefix = file_prefix.upper()
lowercase_file_prefix = file_prefix.lower()

# Write the header guards
cog.outl("#ifndef {}_H".format(uppercase_file_prefix))
cog.outl("#define {}_H".format(uppercase_file_prefix))
]]]*/
//[[[end]]]

#include <stdint.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
//DOM-IGNORE-END

/*[[[cog
# Print comments and other definitions
if (len(comment_strings) > 0):
    cog.outl("{}".format(comment_strings))
cog.outl("#define {}_BUFFER_LENGTH {}".format(
    uppercase_file_prefix,
    len(data_array)))
]]]*/
//[[[end]]]
const uint16_t
/*[[[cog

cog.outl("{}_BUFFER[{}_BUFFER_LENGTH] = {}".format(
    uppercase_file_prefix,
    uppercase_file_prefix,
    "{"))
for data_value in data_array:
    cog.outl("    {}u,".format(data_value))
]]]*/
//[[[end]]]
};

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
};
#endif
//DOM-IGNORE-END

/*[[[cog
# Write the end of header guards
cog.outl("#endif //{}_H".format(uppercase_file_prefix))
]]]*/
//[[[end]]]
