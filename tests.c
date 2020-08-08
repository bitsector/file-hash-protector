#include "tests.h"


void test(){
	unsigned char test[] = "abcd";
	unsigned char hash[SHA512_DIGEST_LENGTH] = {0};
	char *file_contents = NULL;
	size_t contents_len = 0;
	hash_buffer(hash,test,sizeof(test));
	LOG("string:hash %s:%s\r\n",test,hash); 
	memset(hash,0,SHA512_DIGEST_LENGTH);
	hash_a_file(TEST_FILE_1_PATH,hash);
	LOG("%s:%s\r\n",TEST_FILE_1_PATH,hash); 
	print_as_hex(hash,SHA512_DIGEST_LENGTH);
	// TODO why the has is different every time? buffer too big? Note that only the tail of the hash differs
	LOG("%s exists? %d\r\n",TEST_FILE_1_PATH,does_exist(TEST_FILE_1_PATH)); 
	LOG("%s exists? %d\r\n",TEST_FILE_1_PATH"foo",does_exist(TEST_FILE_1_PATH"foo")); 
	
	
}
void test_printf_anomaly(){
	size_t res = 0;
	char str[]="/home/ak/mmn16/test_files/test_file_4:"
				"8ac7093018144d2b0ecfae74b87a659dae19879912f5c67d9d62cd0f88475951cf353f949c32afe116f8a5c66ec69a54da94beb5334d98ed53505f5976d31c7a"
				"/home/ak/mmn16/test_files/test_file_3:"
				"c0a9078aa8d1194bbd92896491a3921b21920cd9efe11db8832d1140c3ced700232810748810265dbacb20d61b9ba8f4f26c65b68d661481ee9ea13598a9dbbd";

	printf("sizeof(str): %d\r\n",sizeof(str));
	printf("strlen(str): %d\r\n",strlen(str));
	res = printf("%s",str);
	printf("\r\n");
	printf("chars printed: %d\r\n",res);
	
}
void test_get_file(){
	size_t len = 0;
	char* file_buffer = NULL;

	file_buffer = get_file(HASH_FILE_PATH,&len);
	if (file_buffer){
		LOG("get_file(HASH_FILE_PATH,&len) retuned ok");
	}else{
		LOG("get_file(HASH_FILE_PATH,&len) failed");
	}
	free(file_buffer);
	file_buffer = NULL;

	file_buffer = get_file(TEST_FILE_1_PATH,&len);
	if (file_buffer){
		LOG("get_file(TEST_FILE_1_PATH,&len) retuned ok");
	}else{
		LOG("get_file(TEST_FILE_1_PATH,&len) failed");
	}
	free(file_buffer);
	file_buffer = NULL;

	file_buffer = get_file("foo",&len);
	if (file_buffer){
		LOG("get_file(\"foo\",&len) retuned ok");
	}else{
		LOG("get_file(\"foo\",&len) failed");
	}
	free(file_buffer);
	file_buffer = NULL;

	file_buffer = get_file("/",&len);
	if (file_buffer){
		LOG("get_file(\"/\",&len) retuned ok");
	}else{
		LOG("get_file(\"/\",&len) failed");
	}
	free(file_buffer);
	file_buffer = NULL;

	file_buffer = get_file("",&len);
	if (file_buffer){
		LOG("get_file(\"\",&len) retuned ok");
	}else{
		LOG("get_file(\"\",&len) failed");
	}
	free(file_buffer);
	file_buffer = NULL;

}
void test_sha_basics(){
	unsigned char test[] = "aalskdjlkjasdlfk";
	unsigned char original_hash[SHA512_DIGEST_LENGTH+1] = {0};
	hash_buffer(original_hash,test,sizeof(test));
    LOG("strlen: %d, original_hash: %s\r\n",strlen(original_hash),original_hash);
	
	
}
// TODO - test this
void test_hex_consistency(){
	char* rc = NULL;
	unsigned char test[] = "abcd";
	unsigned char original_hash[SHA512_DIGEST_LENGTH+1] = {0};
	char hex_res[1024] = {0};
	char str_from_hex[1024] = {0};
    LOG("%s():\r\n", __func__);
	hash_buffer(original_hash,test,sizeof(test));
    LOG("original_hash: %s\r\n",original_hash);
    rc = str_to_hex(original_hash, SHA512_DIGEST_LENGTH,hex_res);
	if (rc != hex_res){
		LOG("digest to hex failed! result returned: %p, hex_res: %p\r\n",rc,hex_res);
		return;
	}

    rc = hex_to_str(hex_res, SHA512_HEX_DIGEST_LENGTH,str_from_hex);
	if (rc != str_from_hex){
		LOG("hex to digest failed! result returned: %p, hex_res: %p\r\n",rc,hex_res);
		return;
	}
    
    LOG("Original character string is: %s, length: %d\r\n", original_hash,SHA512_DIGEST_LENGTH);
    LOG("hex encoded string is: %s, length as buffer: %d, length in theory: %d\r\n", hex_res,strlen(hex_res),SHA512_HEX_DIGEST_LENGTH);
    LOG("back to regular decoded string is: %s, equal to origin: %d, length as buffer: %d, length in theory: %d\r\n", str_from_hex,strncmp(str_from_hex,original_hash,SHA512_DIGEST_LENGTH) == 0,strlen(str_from_hex),SHA512_DIGEST_LENGTH);
}
void test_print_as_hex(){
	char* res = 0;
	char* rc = NULL;
	unsigned char test[] = "abcd";
	unsigned char original_hash[SHA512_DIGEST_LENGTH+1] = {0};
	char hex_res[1024] = {0};
	char str_from_hex[1024] = {0};
    LOG("%s():\r\n", __func__);
	res = hash_buffer(original_hash,test,sizeof(test));
    LOG("hash_buffer return value %d, as pointer %p, original_hash: %d\r\n",res,(char*)res,original_hash);
    LOG("original_hash: %s\r\n",original_hash);
    LOG("\r\n printing as hex: \r\n");
	print_as_hex(original_hash,SHA512_DIGEST_LENGTH);
    LOG("\r\n done \r\n");
    rc = str_to_hex(original_hash, SHA512_DIGEST_LENGTH,hex_res);
	if (rc != hex_res){
		LOG("digest to hex failed! result returned: %p, hex_res: %p\r\n",rc,hex_res);
		return;
	}

    rc = hex_to_str(hex_res, SHA512_HEX_DIGEST_LENGTH,str_from_hex);
	if (rc != str_from_hex){
		LOG("hex to digest failed! result returned: %p, hex_res: %p\r\n",rc,hex_res);
		return;
	}
    
    LOG("Original character string is: %s, length: %d\r\n", original_hash,SHA512_DIGEST_LENGTH);
    LOG("hex encoded string is: %s, length as buffer: %d, length in theory: %d\r\n", hex_res,strlen(hex_res),SHA512_HEX_DIGEST_LENGTH);
    LOG("back to regular decoded string is: %s, equal to origin: %d, length as buffer: %d, length in theory: %d\r\n", str_from_hex,strncmp(str_from_hex,original_hash,SHA512_DIGEST_LENGTH) == 0,strlen(str_from_hex),SHA512_DIGEST_LENGTH);
}
void test_hash_a_file(){
	void* rc = NULL;
	unsigned char original_hash[SHA512_DIGEST_LENGTH+1] = {0};
	unsigned char hex_res[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	rc = hash_a_file(TEST_FILE_1_PATH,original_hash);
	if (rc != original_hash){
		LOG("hash_a_file() failed: %p, hex_res: %p\r\n",rc,original_hash);
		return;
	}
	
	LOG("hash: %s\r\n, converting to hex...\r\n",rc);
    rc = str_to_hex(original_hash, SHA512_DIGEST_LENGTH,hex_res);
	if (rc != hex_res){
		LOG("digest to hex failed! result returned: %p, hex_res: %p\r\n",rc,hex_res);
		return;
	}
	
	LOG("hash_a_file() results: %s\r\n",rc);
	
}
void test_get_path_and_hash_line(){
	char dst[PATH_MAX+SHA512_HEX_DIGEST_LENGTH+4+1] = {0};
	get_path_and_hash_line(TEST_FILE_1_PATH,dst);
	get_path_and_hash_line(TEST_FILE_2_PATH,dst);
}
void test_add_file_to_hash_list(){
	reset_hash_file(); 
	add_file_to_hash_list(TEST_FILE_4_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_1_PATH);
}
void test_write_to_file(){
	char text[] = "hello :)\r\n";
	write_to_file(HASH_FILE_PATH,text);
}
void test_reset_hash_file(){
	test_write_to_file();
	// no the file has some content, let's reset it:
	reset_hash_file(); 
}
void test_is_in_file(){
	reset_hash_file(); 
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	add_file_to_hash_list(TEST_FILE_4_PATH);
	LOG ("is \"file_1\" in file: %d\r\n",is_in_file(HASH_FILE_PATH,"file_1"));
	LOG ("is \"123456\" in file: %d\r\n",is_in_file(HASH_FILE_PATH,"123456"));
	LOG ("is \"\" in file: %d\r\n",is_in_file(HASH_FILE_PATH,""));
	LOG ("is \"dd\" in file: %d\r\n",is_in_file(HASH_FILE_PATH,"dd"));
}
void test_add_file_to_hash_list_w_dups(){
	size_t len = 0;
	reset_hash_file(); 
	write_to_file(TEST_FILE_1_PATH,"goo1111111\n");
	add_file_to_hash_list(TEST_FILE_1_PATH);
	write_to_file(TEST_FILE_1_PATH,"foo111\n");
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	write_to_file(TEST_FILE_1_PATH,"987655\n");
	add_file_to_hash_list(TEST_FILE_1_PATH);
	write_to_file(TEST_FILE_2_PATH,"12345\n");
	add_file_to_hash_list(TEST_FILE_2_PATH);

	
}
void test_get_hash_of_file_from_list(){
	reset_hash_file(); 
	char dst_1[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char dst_2[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char dst_3[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char dst_4[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	add_file_to_hash_list(TEST_FILE_4_PATH);
	get_hash_of_file_from_list(TEST_FILE_1_PATH,dst_1);
	get_hash_of_file_from_list(TEST_FILE_2_PATH,dst_2);
	get_hash_of_file_from_list(TEST_FILE_3_PATH,dst_3);
	get_hash_of_file_from_list(TEST_FILE_4_PATH,dst_4);
	
	LOG("%s->%s\r\n",TEST_FILE_1_PATH,dst_1);
	LOG("%s->%s\r\n",TEST_FILE_2_PATH,dst_2);
	LOG("%s->%s\r\n",TEST_FILE_3_PATH,dst_3);
	LOG("%s->%s\r\n",TEST_FILE_4_PATH,dst_4);
}
void test_get_dir_from_path(){
	char* res = NULL;
	char dir_path[PATH_MAX] = {0};
	res = get_dir_from_path(TEST_FILE_1_PATH,dir_path);
	LOG("res: %p, final result: %s\r\n",res,dir_path);
	res = get_dir_from_path("/home/ak/mmn16/test_files/",dir_path);
	LOG("res: %p, final result: %s\r\n",res,dir_path);
	res = get_dir_from_path("/home/ak/mmn16/test_files",dir_path);
	LOG("res: %p, final result: %s\r\n",res,dir_path);
}
void test_is_path_in_hash_file(){
	reset_hash_file(); 
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	add_file_to_hash_list(TEST_FILE_4_PATH);
	LOG("is_path_in_hash_file(TEST_FILE_1_PATH): %d\r\n",is_path_in_hash_file(TEST_FILE_1_PATH));
	LOG("is_path_in_hash_file(TEST_FILE_2_PATH): %d\r\n",is_path_in_hash_file(TEST_FILE_2_PATH));
	LOG("is_path_in_hash_file(TEST_FILE_3_PATH): %d\r\n",is_path_in_hash_file(TEST_FILE_3_PATH));
	LOG("is_path_in_hash_file(TEST_FILE_4_PATH): %d\r\n",is_path_in_hash_file(TEST_FILE_4_PATH));
	//LOG("is_path_in_hash_file(\"\"): %d\r\n",is_path_in_hash_file(""));
	LOG("is_path_in_hash_file(\"1234\"): %d\r\n",is_path_in_hash_file("1234"));
	LOG("is_path_in_hash_file(\"/home/ak/mmn16/test_files\"): %d\r\n",is_path_in_hash_file("/home/ak/mmn16/test_files"));
}
void test_is_dir_path(){
	LOG("is_dir_path(\"\"): %d\r\n",is_dir_path(""));
	LOG("is_dir_path(\"/home/ak/mmn16/test_files/\"): %d\r\n",is_dir_path("/home/ak/mmn16/test_files/"));
	LOG("is_dir_path(\"/home/ak/mmn16/test_files\"): %d\r\n",is_dir_path("/home/ak/mmn16/test_files"));
	LOG("is_dir_path(\"/home\"): %d\r\n",is_dir_path("/home"));
	LOG("is_dir_path(\"/home/\"): %d\r\n",is_dir_path("/home/"));
	LOG("is_dir_path(\"/\"): %d\r\n",is_dir_path("/"));
	LOG("is_dir_path(\"/home/ak/mmn16/test_files/test_file_2\"): %d\r\n",is_dir_path("/home/ak/mmn16/test_files/test_file_2"));
	LOG("is_dir_path(\"/foo\"): %d\r\n",is_dir_path("/foo"));
	LOG("is_dir_path(\"/foo/\"): %d\r\n",is_dir_path("/foo/"));
	
}
void test_remove_path_from_hash_list(){
	reset_hash_file(); 
	LOG("remove_path_from_hash_list(TEST_FILE_1_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_1_PATH));// should fail
	LOG("remove_path_from_hash_list(\"/foo\"): %d\r\n",remove_path_from_hash_list("/foo"));// should fail
	add_file_to_hash_list(TEST_FILE_1_PATH);
	LOG("remove_path_from_hash_list(TEST_FILE_2_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_2_PATH));// should fail
	LOG("remove_path_from_hash_list(TEST_FILE_1_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_1_PATH));// should succeed
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_1_PATH);
	LOG("remove_path_from_hash_list(TEST_FILE_3_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_3_PATH));// should fail
	LOG("remove_path_from_hash_list(TEST_FILE_2_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_2_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_1_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_1_PATH));// should succeed
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_1_PATH);
	LOG("remove_path_from_hash_list(TEST_FILE_1_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_1_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_2_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_2_PATH));// should succeed
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	LOG("remove_path_from_hash_list(TEST_FILE_2_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_2_PATH));// should succeed
	add_file_to_hash_list(TEST_FILE_2_PATH);
	LOG("remove_path_from_hash_list(TEST_FILE_2_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_2_PATH));// should succeed
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	LOG("remove_path_from_hash_list(TEST_FILE_1_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_1_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_2_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_2_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_3_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_3_PATH));// should succeed
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	LOG("remove_path_from_hash_list(TEST_FILE_3_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_3_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_2_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_2_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_1_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_1_PATH));// should succeed
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	LOG("remove_path_from_hash_list(TEST_FILE_2_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_2_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_3_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_3_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_1_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_1_PATH));// should succeed
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	LOG("remove_path_from_hash_list(TEST_FILE_2_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_2_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_1_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_1_PATH));// should succeed
	LOG("remove_path_from_hash_list(TEST_FILE_3_PATH): %d\r\n",remove_path_from_hash_list(TEST_FILE_3_PATH));// should succeed
	
}
void test_logging_macro(){
	char* msg1 = "some string";
	char* msg2 = "another string";
	LOG("hello");
	LOG("\r\n hello \r\n");
	LOG("some text %s and some another test %s",msg1,msg2);
	LOG("some text: %s, and some another text: %s",msg1,msg2);
	
}
void test_get_line_num_in_file(){
	reset_hash_file(); 
	LOG("get_line_num_in_file(HASH_FILE_PATH): %d",get_line_num_in_file(HASH_FILE_PATH));
	add_file_to_hash_list(TEST_FILE_1_PATH);
	LOG("get_line_num_in_file(HASH_FILE_PATH): %d",get_line_num_in_file(HASH_FILE_PATH));
	add_file_to_hash_list(TEST_FILE_2_PATH);
	LOG("get_line_num_in_file(HASH_FILE_PATH): %d",get_line_num_in_file(HASH_FILE_PATH));
	add_file_to_hash_list(TEST_FILE_4_PATH);
	LOG("get_line_num_in_file(HASH_FILE_PATH): %d",get_line_num_in_file(HASH_FILE_PATH));
	add_file_to_hash_list(TEST_FILE_3_PATH);
	LOG("get_line_num_in_file(HASH_FILE_PATH): %d",get_line_num_in_file(HASH_FILE_PATH));
	reset_hash_file(); 
	LOG("get_line_num_in_file(HASH_FILE_PATH): %d",get_line_num_in_file(HASH_FILE_PATH));
	add_file_to_hash_list(TEST_FILE_3_PATH);
	add_file_to_hash_list(TEST_FILE_4_PATH);
	LOG("get_line_num_in_file(HASH_FILE_PATH): %d",get_line_num_in_file(HASH_FILE_PATH));
	
	
}
void test_string_stuff(){
	
	char dst[100] = {0};
	char s1[] = {'a','b','c','d','\0'};
	
	LOG("s1 itself: %s",s1);
	LOG("strlen(s1): %d",strlen(s1));
	
	snprintf(dst,2,"%s",s1);
	LOG("dst: %s",dst);
	LOG("strlen(dst): %d",strlen(dst));
	for (int i=0;i<10;i++){
		printf("%d: %d=%c, ",i,dst[i],dst[i]);
	}
	LOG("");
	//printf("\\0 : %d\n", '0');
	
	
	
}
void test_hash_a_file_as_hex(){
	char hex_hash_1[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char hex_hash_2[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char hex_hash_3[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	char hex_hash_4[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	hash_a_file_as_hex(TEST_FILE_1_PATH,hex_hash_1);
	hash_a_file_as_hex(TEST_FILE_2_PATH,hex_hash_2);
	hash_a_file_as_hex(TEST_FILE_3_PATH,hex_hash_3);
	hash_a_file_as_hex(TEST_FILE_4_PATH,hex_hash_4);
	LOGS("%s",hex_hash_1);
	LOGS("%s",hex_hash_2);
	LOGS("%s",hex_hash_3);
	LOGS("%s",hex_hash_4);
	LOGS("try manually:");
	
	char hex_hash_short[SHA512_HEX_DIGEST_LENGTH] = {0};
	char hex_hash_long[SHA512_HEX_DIGEST_LENGTH+1] = {0};
	unsigned char temp_hash_short[SHA512_DIGEST_LENGTH] = {0};
	unsigned char temp_hash_long[SHA512_DIGEST_LENGTH+1] = {0};
	
	hash_a_file(TEST_FILE_1_PATH,temp_hash_short);
	hash_a_file(TEST_FILE_1_PATH,temp_hash_long);
	
	str_to_hex(temp_hash_short,SHA512_DIGEST_LENGTH,hex_hash_short);
	str_to_hex(temp_hash_long,SHA512_DIGEST_LENGTH,hex_hash_long);

	LOGS("%s",hex_hash_short);
	LOGS("%s",hex_hash_long);

}
void test_finallize(){
	stop_fs_notifications();
}
void test_check_all_existing_hashes(){
	int res = 0;
	reset_hash_file(); 
	check_all_existing_hashes(); // test on empty hash file
	add_file_to_hash_list(TEST_FILE_1_PATH);
	check_all_existing_hashes(); // test on one file
	reset_hash_file(); 
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	add_file_to_hash_list(TEST_FILE_4_PATH);
	check_all_existing_hashes(); // test on few file
	reset_hash_file(); 
	add_file_to_hash_list(TEST_FILE_1_PATH);
	write_to_file(TEST_FILE_1_PATH,"foo012983"); // change file after getting it's hash
	check_all_existing_hashes(); // test on one file already changed
	reset_hash_file(); 
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	add_file_to_hash_list(TEST_FILE_4_PATH);
	write_to_file(TEST_FILE_3_PATH,"alsdkjf;lafkd"); // change file after getting it's hash
	check_all_existing_hashes(); // test on one file already changed
	reset_hash_file(); 
	write_to_file(TEST_FILE_1_PATH,"1");
	write_to_file(TEST_FILE_2_PATH,"2");
	write_to_file(TEST_FILE_3_PATH,"3");
	write_to_file(TEST_FILE_4_PATH,"4");
	add_file_to_hash_list(TEST_FILE_1_PATH);
	add_file_to_hash_list(TEST_FILE_2_PATH);
	add_file_to_hash_list(TEST_FILE_3_PATH);
	add_file_to_hash_list(TEST_FILE_4_PATH);
	write_to_file(TEST_FILE_1_PATH,"11");
	write_to_file(TEST_FILE_2_PATH,"22");
	write_to_file(TEST_FILE_3_PATH,"33");
	write_to_file(TEST_FILE_4_PATH,"44");
	check_all_existing_hashes(); // test all files modified


}
void test_syslog(){
	syslog(LOG_WARNING,"hash shield hello\n");
	LOGE("LOGE() test, did it worke?");
	LOGS("LOGS() test, did it worke?");
	LOG("LOG() test, did it worke?");
	LOG_ATTENTION("ATTENTION:this should be in red");
	LOG_OK("this should be in green");
}	

void run_all_tests(){
	//test_get_file();
	//test_sha_basics();
	//test_hex_consistency();
	//test_print_as_hex();
	//test_hash_a_file();
	//test_get_path_and_hash_line();
	//test_add_file_to_hash_list();
	//test_write_to_file();
	//test_reset_hash_file();
	//test_is_in_file();
	//test_add_file_to_hash_list_w_dups();
	//test_get_hash_of_file_from_list();
	//test_get_dir_from_path();
	//test_is_path_in_hash_file();
	//test_is_dir_path();
	//test_logging_macro();
	//test_hash_a_file_as_hex();
	//test_finallize();
	//test_remove_path_from_hash_list();
	//test_check_all_existing_hashes();
	//test_string_stuff();
	//test_get_line_num_in_file();
	//test_printf_anomaly();
	//test_get_line_num_in_file();
	
	test_syslog();
}


