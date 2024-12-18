import os
import shutil
import argparse

def copy_files(src, dest):
    """Copy files from src to dest, skipping files that already exist."""
    if not os.path.exists(dest):
        os.makedirs(dest)
    for src_dir, dirs, files in os.walk(src):
        dst_dir = src_dir.replace(src, dest, 1)
        if not os.path.exists(dst_dir):
            os.makedirs(dst_dir)
            print(f"Created directory {dst_dir}")
        for file_ in files:
            src_file = os.path.join(src_dir, file_)
            dst_file = os.path.join(dst_dir, file_)
            if os.path.exists(dst_file):
                print(f"Skipping {src_file} as it already exists in destination.")
                continue
            # Copy file if it does not exist in the destination
            try:
                shutil.copy(src_file, dst_file)
                #print(f"Copied {src_file} to {dst_file}")
            except:
                print(f"Error copying {src_file}")
                exit()

def main():
    parser = argparse.ArgumentParser(description='Recursively copy files from source to destination, skipping files that already exist.')
    parser.add_argument('--source', required=True, help='Source directory to copy files from')
    parser.add_argument('--dest', required=True, help='Destination directory to copy files to')
    args = parser.parse_args()
    copy_files(args.source, args.dest)

if __name__ == "__main__":
    main()

