#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OSC_MAX_STR_LEN 768

char  **build_osc_str(int *osc_str_len, char *str) {
	/* split string into >=768 byte chunks, OSC on top of UDP will truncate
	 * payloads that are too big. i'm not sure what this limit is, but
	 * 768 bytes feels good to me */
	char *tmp, **osc_str, **osc_str_tmp = NULL;
	int idx, osc_str_idx = 0;
	
	if(strlen(str) > OSC_MAX_STR_LEN) {
		for (idx=0; idx<strlen(str); idx+=OSC_MAX_STR_LEN) {
			if ((strlen(str) - idx) < OSC_MAX_STR_LEN) {
				tmp = malloc(sizeof(char) * (strlen(str) - idx + 2));
				memcpy(tmp, str+idx, strlen(str) - idx + 1);
			} else {
				tmp = malloc(sizeof(char) * OSC_MAX_STR_LEN+1);
				memcpy(tmp, str+idx, OSC_MAX_STR_LEN);
			}
			
			if(!osc_str_idx)
				osc_str = malloc(sizeof(char *));
			else {
				osc_str_tmp = realloc(osc_str, sizeof(char *) * (osc_str_idx + 1));
				if (osc_str_tmp == NULL) {
					printf("could not allocate memory!");
					exit(1);
				}
				osc_str = osc_str_tmp;
			}
			osc_str[osc_str_idx] = tmp;
			osc_str_idx += 1;
		}
		*osc_str_len = osc_str_idx;
	} else {
		osc_str[0] = malloc(sizeof(char *));
		*osc_str_len = 1;
		strcpy(osc_str[0], str);
	}
	return osc_str;
}

int main(void) {
	int i = 0;
	int strlen = 5168;
	char str[5169] = "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";

	char str_chunk0[769] = "0EAHqSIuCe4b7HRlk9XyXAN0uq7L7WNb1x3QEcuCurLV0rxxZLQVYiNqV5xArgr0hzGEt9JHIHaO2v973ZV8ps76gvD9Hk4WxxHWOAA7HVJNuWQfESwwQQHIanhG4jHAMPczevMFlqL3MKAF6f0BOM13dXf73S6igmzUXJBMDumJQ7rae8SdZs7W5HCC9xVhkLEn6X6MdplfIaen7RYhQqMK7aNO1vDmw4XAzR18n7RnliixDilKxhckKjZ127nP3Kb2n2jdcjUIymcVG1iHa7NLuqL3DxvqLkC0HLDNc1Eaj9DzM4JWPjxGhXeEYvn0HgYbqqtVvnvKwidROEWvQFLuKVinJH3OzLJN6PjV8VM7UdtKvRj2DQmbuBdkniHXVLrU5bcNzXKYREwfdeCXreAopZtAr1xUdVPxOPCFuqpcxdgLOT4BPfbtssSezhrY0i83mgkQlj3LkYmToqh9C0HeNPuXhMeDX3aYELnjeXAGDriOZK5lQv88hNquV4WnyeyuI90cWDlcLEF3xPTvtFZXN0KEikjJoMLl7le5FMaEkURbwsktkxeW01Qhdh817czaEjvqkKARdVIT3SkqfB9cYFBWhrG9fhZ9gFOBcMsyjDDqdLhSXBUPp8d8c8Hkaa1UbYtpabd7sRWyTQP4IzHHmjOOkr5RpY5mrXnYR7msxfIsJRel5Jaog0RRDLfQCwx1q5HJqdAyeJrZ8ZYIPJTEcq5TXieSpF8QxLL4WsN8cCLCch1MXLIBlaSbIgRh";

	char str_chunk1[769] = "UnBY8L57t54VMwiEdMRzQxSR4sCjgH6mUhUH2rBRIeelXWRHs0VhDm5yjokVLdtwDvs5BXrRbGjXf9JGeoHHr0WDeySH8mob3kBZaY3fRV1nMTmeaq6ALZENpcKungbFpMoYV0rrxAh0zXvzyW4vNTUSfugbenB5lGcFSI0oxt3JtFquLLQDoFsjN4TKbadnAw8GGxTrS3yMD9w4KTpEra8NQzo86pM1vHptakCoyhRBtr1SZwci1UCMawJCePXXpuuElpP45h8NYGATiuK35hQhZuuQvsZDfgOELilEjEC5njgxlTIAzFyEGRtgUFb3WJoBKfNNCQb7ZQnrEmfRlC8wFkia4vdIL53yqngkSH6zmB5DGiLVneUGxg9y1sX01FspfrjV831LLPfk9sme3julpwi5JvdYznbRrt8IWNNg4sNGWlPeLWEZctIICqa9j6kZclb4v1tdacdJpp5zKZLG4KQjEwHgJUfqVWdjw7WY8Jwmy9knQjczxCvlyR4ypR4sHjBHoYag1tQDhghw4ZUNs5rilAna1ViSCjXbcxgt01anqQhFFkKAvkAwAkpH1Uv9MWOmv4EZ6WHyIbO6vGmc5NxHccQHT4Hq1Jl8rjztRXOrIoK3jWWYutgBEPu4T3gdVolDZqRrYI5uKYz9RvOStqDreC138LrYRRePw6ttGBtdi1FiQ6dJr4Uq3o3tdBnwA3Dnl1rW9glPPuPLS3UOgrKqIklEgIPLOP7hQp9yKxyCiDJfwUaXVVUanLih";

	char str_chunk2[769] = "OTMpNEtq3s4N0IJTvEkCTv5AoGXwvtHkI48S1P3V31PJfDVCfsfW6d5LtmrEvQwEQRxuhg8NJo2dhRBcSOkVEGIe5oilJYRDIqQlAYMXQ4xWP17JOs42oklfeNCWX1DmqXHUsVdSgdNQywxx7OFrhVz7FwSA06Z5VfJowxGuFWDmQcH9TWnVCAk6wSKqJQzadOqG6YlxNTiSJEDapXzGtNTKJagCrrxNmhiPOSedC7aGYawbllzxWwjklEqbRJn3CKgAm12u1VkY5M0ELqxqUem3Uw2FpYimWSaeQw5UwLjDYk6m5SXpIZma8aXEemBWFpmTBHyU1KACDBLCkgzruRULwUlZ9hkQF4VCMF9Q0T8kKp6W00WaMUndR900SPPp5k9plOvbUycOLOIcppGMFUPVjVAJXMFWDu84lICBY8w0cGKagbUXWOOsAxNOAdC16rioEFoJAyPv6G1h32OLrWyfPcI5KqBVrsXF23PHYS1K6folhsdsmZui75hoFRBDNlpvtbTvjzKTYNAgxLBcbZslDcPDlKImapdaQdchDEToN40GBISIJ8S68a5TcCPze6PLT2Ce94bx2ZukktwI0gVT3RA40gbGEECf9IV2bURXoa4How9zcNO5TKy5X0dlZ6WFis6drGNL93EmEfGjsqPGfi7jj8EnoNOGBRdEgldK858rX2VeDoiXhS8S9iUjPgpnY0JAygzbZeSbiCCe4Re99YGcR9wtUls30MzGjQNiItuLjgdAp2wI0t6HPX8B";

	char str_chunk3[769] = "hymgc0gpy07pEydRe2UJ82Cfid0fapw0chQgJroM1HJmiaaRmDuQPBKVqTcmIzda2LdnmscR4cl8vYnpEtSZwBfkxsGVET3qX4kJ4tq1zxaplocAlY4evwHDlFN7XaKs8OXNWUtsXNqPYZ1bh7KLjR3OKVsDI641SWARjDVililrQkHWZ8hzbjOIQ4ZKoWSB7ozbIVf46yotu3Q1apFA3ne86KGI4bchis2TQfr7e5D4pq1rYrdhA0Emjh2UAbiIkPbdZgjnYfcNy1t1KgqPyjkG5cQlnWPmuSW35K92ynbeQAfwdTdsKsSV5IeVUcMyOw44NGd46k97NnNZEp9uTnTx8TLlFRBwTPMErUOODLzN4t1rOpf7SqDox7twoCNKAjjsVcQec7j2TRZil5UmLMDCSqcGtPAcx5jY7vLixDuR6UXRGjVG6XZiJYI6mFClrjGLQuetUWcAa9QsMT2KPNkIh6wPRqUUOY8kSvVXo8aa4SZPPJJt9vw9t1Dlrc0Bnfv2KF5FCZVT98qALWANZuu1sjqJC9evTnrc98vzSzsZ06FpJK4Dy0lHb8ZWafqvq4g4pdgoPZw1NbYNJmzRttXFmMty2dXkEmY0q8kUa1KF2O1HhZHtObkHitlbmiKCGSoLQXNSlJNBv9dYYfW5v5MO0n2akylpxthgzr3zEoBwBh9YwdsxP0HiXEP4raJ72n4CUd8c6zTTMeDyhztrq0n3HIpKn4H3LSTnUob6tApYkLysmHx0RaPetvcNPUA1";

	char str_chunk4[769] = "sTYhbjXh6qUQETr7TgLwBJpE5zOSnE1XXQalZa5YY5HKTYCuOUOxWQMBNc6Bag6TPQwNA2VJ2S5FUT6RVFPFvDSyWRc7VPI4DpJOstqrTGi5VYSLpfHmzgT467G8IEnZhgAkWXXqECtKXKCYF0VrUNbR5SKhgMN9AunM3Aei8fI4lS7aG9vRgKnFyuztFHRxtmrHgq1dxEwp5UlQ2OBoIfGwZMQolZpgaA3URkWKh38A6iIm9f8lxDvqOHiKihdhsLykstwXiR4kvtVIVfPYgYYltmHEbKD491ESE1FBi7W5dY0titHctQq0cij9tsJatcOcHaEOLiVns08aWTqhcqFzQ0Y3ZiBayk6Hiay1ZI2PYykI4EQjez3VdgdFAc5qMSYBaDMMlZQGKmWTuLYrKezZB3BWFV3V9a7yDVxa70KuOuQ1jRHpdyI3IIFtPtA9JqcBEzhaskINjTQqz7lp7lmEK0yb7kDU6Lj3Tnsr8ya0dAbhuWgYVTJchTlvPbo3qIGsUjWqa4oF17NvYfeTdeM7Si5MObQoMFDI8HeRXQj7lcySLKwUSCrGnUkNBrE0DVKyVu7vGGOr19kQqXSQh0OmUa0WxD8dOCAy00Sx7zxkotsZ68Gkb8eDhWWuOHrjjfPwK8jYjP7C1qy20HuFcfFmry9PBHKQOe82FULmuSawCwGmPKuNlv9ADuIXsUjBmhmqMl2XuvEBEPB6PV2axw7zsNFBPegrZljD0gbTnA4NAtPQcD7wOLdd70nY2tpH";

	char str_chunk5[769] = "6wombsZdUV60dphrWIYWkjh6ZUfUTXSQmvSEBb2QylwN3fgjOTNgVrVSU4NYfxePXg1s5x3Lt7mXbfTCS6eQGXRTi5tVy84L6TmEC6UZRok0AUboHvLMTfXAnBzOEgkFSBIgnbWxCy0JqINIikIwf3hWgLiawgbqiYYjZPHJ1Qi830fEssUtnjivCOwCfYe994ZBqCO4M82NMhFvMr6zpAd8Y1ZYjo3fvmsKej8cjfgCaxuMTAsZD8v7xYQoK9l1PwEZ7BZoxSHHJq08mT4j87YEyjSRPGBI016nhL9ygLmR2z5wPDEDfYlhhTKBeFWCNqYYINUBPqDQSrqQTrKQZGFDL3VnjZH9o2Ig6VR9vGD44ZnlC3PR5U01uG6Wgp8Ax4fw9xZUDGmY2D33ZdmQCFOJXFWaqtFyoLPjcSiLkNKLPolD8ujwJyo0c6q9u4QBJvjJwP0etDwAxqktKhP9qEUjDqj0WZ5XHYZRWMvZmfKHTwVZiicJsRxmoFAdArqj4EXuKWrbpf8Pf6NTWc6FIgYpAu7qDmu73RFdC89i8xM2i8Ac9ZxmUFsVxynToHM2SlUY1Sk9VWI6nqJQWHEfuILcK1piDqxgB5JiW7c7uZxrHB0jgrnMGEAgrNvbobd0jSFOtLeHmRA4MuNRmHCwsiCh4G7hVNOzcVEkS5jKwvKEE1p4VObgDAOjf2O57Bu8DSMjUNgZ1sNflaGj2VGYYEXj9VzmRzNofW6HKVUKWlWcViYSvqI3BafIQq532OTG";

	char str_chunk6[561] = "mjshLs486mCn24QWrRzfR6Kostq0M2tiK1uRSlD2ZOzTVDSX4nn8Jfndzxub4jekS9DLb0R14Uv8Bra0RLe4iqaT4V7FcajYlJli7k4iPer1LLKIzwdbqgtfia4ZhvuuqQ2zcAvF7khzXopsjTrn5qlMUD3BtnUfViq33U8HT7LuYfrXTGZ3fzLnpOoJXJY4uzohO7TsZXSazxLz4INUI2SHEmcNDuBuXH1DJozaMXnmjy2iehXha5VDcOVI2lsZJeXyuXTgbSkm7T12xzzp8QgVuoAwOPVmcOMiNgegPLVdkkHMBJPESqRA6tItfDLrNhfZYzonkD1EAjJCL6FR0I8L2K4xHh0ppXXf2HG9EdT0KrHYxThJo7FjfKc7B8kkQRHcFvO6eZ92sJEmCg3pQoYopiCuhCuDdsanIIRY6wSsPnAH9SbtFLbWOytjNNcJnwySQI0RlmxbLvIh77NT6oYP9d0EgMX5iDOSrYBjwYW3wPnSU0SnY2BtXqF79D4OtzCID511OgIzrtxTG3tF8gvhHqFjzspP0B5fDU4Uo0bitS8j";

	int chunk6len = 561;
	
	char str_short[368] = "2KD36aHSbf1F7Nk7K167Kbyez3DOY3oGp8wHFIswrRjTbG1nnRVMXOOlhCW4hQte1IJKsOtj1r8tWegeIlcNQxw6AxeQGvaHOy5fx0X0NRMJSWXNQCcw4jF9M6Dpe4tVOQYajdzoYMK3jX2RygY1XQpktwnXxejnGgp8GvRWzVGu4ETDhzckxwMZLrQjSxrnSfY6Id2Z7FZqQlbRlFQqRuW1Z7jK4XpTCgB4d8t6syIC8DEGdJQCLIh7LWT1AmzfAz1y2O8E0gdKO79mKob2PZAEpCE1LCrJtGJQde32NAWYHDTFpBZ4G4RidTaMSj734WR64Wimwh8pqWKMWVmfc7O1RLeh7XYGwqWffZE83lT0xvm";
	
	int str_shortlen = 367;
	
	char **osc_str = NULL;
	int osc_str_len = 0;

	bool error;

	error = false;
	osc_str = build_osc_str(&osc_str_len, str);

	printf("build_osc_str() returned %d string(s)\n", osc_str_len);
	for(i=0; i<osc_str_len; i++) {
		printf("i: %d -- %s\n\n", i, osc_str[i]);		
		switch(i) {
		case 0:
			if (strcmp(str_chunk0, osc_str[i])) {
				printf("corrupted string!! i: %d, s: %s\n", i, osc_str[i]);
				error = true;
			}
			free(osc_str[i]);
			break;
		case 1:
			if (strcmp(str_chunk1, osc_str[i])) {
				printf("corrupted string!! i: %d, s: %s\n", i, osc_str[i]);
				error = true;
			}
			free(osc_str[i]);
			break;			
		case 2:
			if (strcmp(str_chunk2, osc_str[i])) {
				printf("corrupted string!! i: %d, s: %s\n", i, osc_str[i]);
				error = true;
			}
			free(osc_str[i]);			
			break;
		case 3:
			if (strcmp(str_chunk3, osc_str[i])) {
				printf("corrupted string!! i: %d, s: %s\n", i, osc_str[i]);
				error = true;
			}
			free(osc_str[i]);			
			break;
		case 4:
			if (strcmp(str_chunk4, osc_str[i])) {
				printf("corrupted string!! i: %d, s: %s\n", i, osc_str[i]);
				error = true;
			}
			free(osc_str[i]);			
			break;
		case 5:
			if (strcmp(str_chunk5, osc_str[i])) {
				printf("corrupted string!! i: %d, s: %s\n", i, osc_str[i]);
				error = true;
			}
			free(osc_str[i]);			
			break;
		case 6:
			if (strcmp(str_chunk6, osc_str[i])) {
				printf("corrupted string!! i: %d, s: %s\n", i, osc_str[i]);
				error = true;
			}
			free(osc_str[i]);			
			break;					
		default:
			printf("unknown index! i: %d\n", i);
			error = true;
			break;
						  
		}
		if(error) {
			break;
		}
	}
	free(osc_str);

	if(error) {
		printf("error found! exiting..");
		exit(1);
	}

	error = false;
	osc_str = build_osc_str(&osc_str_len, str_short);
	printf("build_osc_str() returned %d string(s)\n", osc_str_len);	
	printf("i: %d -- %s\n\n", 0, osc_str[0]);		
	if (strcmp(str_short, osc_str[0])) {
		printf("corrupted SHORT string!! s: %s\n", str_short);
	}
	if(error) {
		printf("error found! exiting..");
		exit(1);
	}
	free(osc_str[0]);
}
