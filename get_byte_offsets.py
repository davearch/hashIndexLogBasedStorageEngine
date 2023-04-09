# Open the file in binary mode to read bytes
with open('cmake-build-debug/log.data', 'rb') as file:
    # Loop through the file
    for byte_offset, char in enumerate(file.read()):
        # Print byte offset and corresponding ASCII character
        print(f'Byte offset: {byte_offset}, Character: {chr(char)}')
