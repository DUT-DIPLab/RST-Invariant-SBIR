import os

file_names = []
for file_name in os.listdir('m_parts'):
    file_names.append(file_name)
with open('Part.task', 'w') as writer:
    writer.write(str(len(file_names)) + '\n')
    for file_name in file_names:
        writer.write('0 m_parts/' + file_name + '\n')
    writer.write('0\n')
