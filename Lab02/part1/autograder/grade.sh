#!/bin/bash

# Check if we have enough arguments
if [[ $# -ne 1 ]]; then
  echo "Usage: ./grade.sh <filename>"
fi

# Delete temporary files
if [[ -e "ref/*.out" ]]; then
  rm ref/*.out
fi

# Compile the reference program
gcc ref/*.c -o $1

# Generate reference output files
total=0;
for input in ref/*.in; do
  total=$(($total+1));
  ./fun < $input > "${input%%.*}".out;
done
# Now mark submissions

#
# Note: See Lab02Qn.pdf for format of output file. Marks will be deducted for missing elements.
#

# Iterate over every submission directory
    # Compile C code
    # Print compile error message to output file
    # Generate output from C code using *.in files in ref
    # Compare with reference output files  and award 1 mark if they are identical
# print score for student
# print total files marked.

echo -e "Test date and time: $(date +"%A, %d %B %Y, %X")\n" > results.out;

for dir in subs/*; do
  tmp="${dir##*/}"
  pts=0
  gcc "$dir"/*.c -o "$dir"/"$1" 2> /dev/null
  if [[ $? -ne 0 ]]; then
    echo "Directory $tmp has a compile error" >> results.out;
  else
    for input in ref/*.in; do
      full_file_name="${input##*/}"; # with extension
      file_name="${full_file_name%%.*}"; # without extension
      
      "$dir"/"$1" < $input > "$dir"/"$file_name".out 2>&1
      
      if diff "${input%%.*}".out "$dir"/"$file_name".out > /dev/null
      then
        pts=$(($pts+1))
      fi
    done
  fi
  echo "Directory $tmp score $pts/$total" >> results.out;
done

echo -e "\nProcessed $total files." >> results.out;
