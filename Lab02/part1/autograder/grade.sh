#!/bin/bash

# Check if we have enough arguments
if [ $# != 1 ]; then
  echo "Usage: ./grade.sh <filename>"
else
  gcc ref/*.c -o $1
  for output in ./ref/*.out; do
    if [[ -e $output ]]; then
      rm $output
    fi
  done
  >results.out
  result="results.out"
  echo "Test date and time: $(date +"%A, %d %B %Y, %X")" >> $result
  echo "" >> $result
  tasks=0
  for i in ./ref/*.in; do
    ./$1  < $i > $i.out
    tasks=$((tasks + 1))
  done
  students=0
  for dir in ./subs/*; do
    students=$((students + 1))
    dirname=$(basename $dir)
    gcc ${dir}/*.c -o ${dir}/$1
    if [[ $? -ne 0 ]]; then
      echo "Directory $dirname has a compile error" >> $result
      echo "Directory $dirname score 0/$tasks" >> $result
    else
     for i in ./ref/*.in; do
       inputname=$(basename $i)
       ${dir}/$1 < $i > ${dir}/$inputname.out
     done
     total=$tasks
     for output in ./ref/*.out; do
       outputname=$(basename $output)
       cmp -s $output ${dir}/$outputname
       total=$((total - $?))
     done
     echo "Directory $dirname score $total/$tasks" >> $result
    fi
  done
  echo "" >> $result
  echo "Processed $students files." >> $result
fi
# Delete temporary files

# Compile the reference program

# Generate reference output files

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
    
