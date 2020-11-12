input_file = open('files.txt', 'r')
output_file = open('file_list.txt','w')

count = 20
i = 0
for line in input_file:
    print(line)
    new_line = './Gutenberg/txt/' + line 
    output_file.write(new_line)
    if i > count:
        break
    i += 1


input_file.close()
output_file.close()