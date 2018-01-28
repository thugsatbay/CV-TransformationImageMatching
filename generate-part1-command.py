command = "./a2 part1 "
image_file = open("file_names.txt", 'r')
query_file = open("selected_file.txt", 'r')
image_name_list = []
for image_name in image_file:
	image_name_list.append(str(image_name)[:-1])
query_image = ""
for line in query_file:
	query_image = str(line)[:-1]
image_file.close()
query_file.close()
command += query_image + " "
files = " ".join(map(str,image_name_list))
command += files
command += " > results/Result_" + query_image[:-4] + ".txt" 
output_file = open("run_command.sh","w")
output_file.write(command)
output_file.write("\necho \"Operation Completed\"")
output_file.close()
