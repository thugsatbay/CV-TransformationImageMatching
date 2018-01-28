#!/bin/bash
rm -f selected_file.txt
rm -f run_command.sh
MASTER_FILE="$(shuf file_names.txt | head -1)"
echo "${MASTER_FILE}"
echo "${MASTER_FILE}" > selected_file.txt
python generate-part1-command.py
chmod +777 run_command.sh
