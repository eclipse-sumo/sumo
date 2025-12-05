import os


def update_testsuite_files():
    # Target filenames to look for
    target_files = [
        "testsuite.netedit.external",
        "testsuite.netedit.internal",
        "testsuite.neteditoutput"
    ]

    # Get the directory where the script is located
    base_dir = os.path.dirname(os.path.abspath(__file__))

    print(f"Starting scan in: {base_dir}")

    # Walk through the directory tree
    for root, dirs, files in os.walk(base_dir):
        # Check if any of the target files exist in the current directory
        for filename in target_files:
            if filename in files:
                file_path = os.path.join(root, filename)

                # Get list of subdirectories in the current folder (root)
                subfolders = [
                    d for d in os.listdir(root)
                    if os.path.isdir(os.path.join(root, d)) and not d.startswith('.')
                ]
                subfolders.sort()

                print(f"Updating {filename} in {root}")

                try:
                    with open(file_path, 'w') as f:
                        # Join the list with double newlines and write it at once
                        f.write("\n\n".join(subfolders))
                except IOError as e:
                    print(f"Error writing to {file_path}: {e}")


if __name__ == "__main__":
    update_testsuite_files()
