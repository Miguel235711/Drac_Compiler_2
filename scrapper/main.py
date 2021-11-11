from selenium import webdriver
import re

number_regex = re.compile(r'([+-]?\d+)|\$') #including $
no_tags_regex = re.compile(r'<.*?>')
op_letter_regex = re.compile(r'[rs]')

char_to_int_op = { 
    'r':0
    ,'s':2
}
GoTo = 1
Acc = 3

with open('./grammar.txt','r') as in_obj:
    driver = webdriver.Chrome('./chromedriver.exe')
    driver.get("http://jsmachines.sourceforge.net/machines/slr.html")

    input = driver.find_element_by_id('grammar')
    input.clear()
    input.send_keys(in_obj.readlines())
    driver.find_element_by_xpath('/html/body/table/tbody/tr/td[2]/input').click()
    symbols_header = driver.find_element_by_xpath('/html/body/table/tbody/tr/td[3]/div/table/tbody/tr/td[1]/div/table/thead/tr[3]')
   # print('all',number_regex.findall(symbols_header.get_attribute('innerHTML')))
    symbols = list(map(lambda x : 40 if not x else int(x),number_regex.findall(symbols_header.get_attribute('innerHTML'))))
    #print('symbols',symbols)
    states_and_cells = driver.find_element_by_xpath('/html/body/table/tbody/tr/td[3]/div/table/tbody/tr/td[1]/div/table/tbody')
    with open('../syntactical_table.txt','w') as out_obj:
        rows = states_and_cells.find_elements_by_tag_name("tr")
        out_obj.write(f'{str(len(rows))}\n')
        for state,row in enumerate(rows):
            row_cells = row.find_elements_by_tag_name("td")[1:]
            edges = []
            for cell,symbol in zip(row_cells,symbols):
                content = re.sub(no_tags_regex,'',cell.get_attribute('innerHTML'))
                if content != '&nbsp;':
                    edge = [symbol,Acc,-1]  # special state -1
                    if content!='acc':
                        number_match_result = number_regex.findall(content)
                        op_match_result = op_letter_regex.findall(content)
                        edge[1] = char_to_int_op[op_match_result[0]] if op_match_result else GoTo 
                        edge[2] = number_match_result[0]
                    #print('edge: ',edge)
                    edges.append(edge)
            out_obj.write(f'{len(edges)}\n')
            for edge in edges:
                out_obj.write(f'{edge[0]} {edge[1]} {edge[2]}\n')
                #print('cell:',,'symbol:',symbol)
        #print(states_and_cells)
    print('done')