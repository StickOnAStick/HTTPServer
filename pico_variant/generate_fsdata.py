import os

# === CONFIG ===
HTML_DIR = "public"  # Folder where your HTML lives
OUTPUT_C = "src/generated_fs.c"  # Where the .c output goes
OUTPUT_H = "inc/generated_fs.h"  # Optional: header

# === Script ===

def convert_file_to_c_array(filename):
    var_name = filename.replace(".", "_").replace("/", "_")
    with open(filename, "rb") as f:
        data = f.read()
    array_data = ", ".join(f"0x{b:02x}" for b in data)
    return f"const unsigned char {var_name}[] = {{{array_data}}};\nconst unsigned int {var_name}_len = {len(data)};\n"

def main():
    os.makedirs(os.path.dirname(OUTPUT_C), exist_ok=True)
    os.makedirs(os.path.dirname(OUTPUT_H), exist_ok=True)

    with open(OUTPUT_C, "w") as cfile, open(OUTPUT_H, "w") as hfile:
        cfile.write("#include <stdint.h>\n")
        cfile.write("#include \"generated_fs.h\"\n\n")
        
        hfile.write("#pragma once\n")
        hfile.write("#include <stdint.h>\n\n")

        for root, dirs, files in os.walk(HTML_DIR):
            for file in files:
                path = os.path.join(root, file)
                symbol = path.replace(".", "_").replace("/", "_")

                c_array = convert_file_to_c_array(path)
                cfile.write(c_array)
                hfile.write(f"extern const uint8_t {symbol}[];\n")
                hfile.write(f"extern const unsigned int {symbol}_len;\n")

if __name__ == "__main__":
    main()
