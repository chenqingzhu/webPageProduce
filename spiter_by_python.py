import urllib2
import os,sys,time
import random
url_file_path = "/Users/pc/UrlFile"
html_file_path = "/Users/pc/HtmlFile"
url_dict_file = "/Users/pc/UrlFile/url_dict_file.txt"
max_url_len = 60;
def  get_file_list(path):
	files = os.listdir(path)
	file_list = []
	for f in files:
		if f[0]== '.':
			continue
		else:
			if path[-1] == '/':
				file_list.append(path + f)
			else:
				file_list.append(path + "/" + f)
	return file_list

def get_url_list(path):
	file_list = get_file_list(path)
	url_list = []
	for fl in file_list:
		f = open(fl)
		url_list.extend(f.readlines())

	max_len  = 0
	for url in url_list:
		if len(url) > max_len:
			max_len = len(url)
	max_url_len = max_len
	return url_list


def get_url_html_file(url):
	if(url == ""):
		return ""
	try:
		request = urllib2.Request(url)
		response = urllib2.urlopen(request,timeout=8)
		page = response.read()
		return page
	except Exception ,e:
		print e
		return ""

def get_code_type(page):
	charset_index = page.find("charset")
	codeType = ""
	if charset_index== -1:
		return codeType
	else:
		i = charset_index + 7
		if(i >= len(page)):
			return codeType
		while(i<len(page) and page[i] != '='):
			i += 1
		i += 1
		if(i >= len(page)):
			return codeType
		begin = i
		while(i < len(page) and page[i] != '>' and page[i] != '/' and page[i] != ' ' and page[i] != '\n' ):
			i +=1 ;
		if(page[begin] == '"'):
			begin += 1
		codeType = page[begin:i-1]
		return codeType
	
def get_all_html(urllist):
	url_dict = {}
	index = 0;
	had_produce_num = 0;
	error_num = 0;
	f_url = open(url_dict_file,'w')
	f_get_page_error_url = open("/Users/pc/UrlFile/get_page_error_url.txt",'w')
	f_decode_error_url = open("/Users/pc/UrlFile/decode_error_url.txt",'w')
	for url in urllist:
		'''
		if( had_produce_num > 10):
			f_url.close()
			f_decode_error_url.close()
			f_decode_error_url.close()
			break;
		'''
		if url in url_dict:
			had_produce_num += 1
			continue;

		print "total url num: ",len(urllist),"  had produce: ",had_produce_num,"   error num:",error_num,"   url==> ",url

		page = get_url_html_file(url)
		if(page==""):
			print "get_url_error"
			f_get_page_error_url.write(url + "\n")
			error_num +=1
			had_produce_num +=1
			continue
		print "get_url_html_file ok"

		codeType = get_code_type(page)
		if(codeType == ""):
			print "codeType null"
			f_decode_error_url.write(url + "\n")
			had_produce_num +=1
			error_num +=1 
			continue
		else:
			try:
				file_path = html_file_path+ "/" + str(index) +".html"
				files = open(file_path,'w')
				page = page.decode(codeType).encode("utf-8")
				print "decode page ok"
				files.write(page)
				files.close()
				url_dict[url] = file_path
				w_str = url[0:-1] + "#3#"+ file_path + "\n"
				f_url.write(w_str)
				index = index + 1
				had_produce_num +=1
				print "write html file ok"
			except:
				error_num += 1
				had_produce_num +=1
				f_decode_error_url.write(url + "\n")
				print "decode page error"
				continue
	f_decode_error_url.close()
	f_url.close()
	f_get_page_error_url.close()

urllist = get_url_list(url_file_path)
print max_url_len
time.sleep(3)
random.shuffle(urllist)
get_all_html(urllist)