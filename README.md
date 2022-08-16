#### Example

```
#include "lazycurl.h"

int main(){
	lazycurl::setup_files();

	while (true) {
		std::cout << lazycurl::curl("-l -k https://raw.githubusercontent.com/CowNation/CowSpeak/master/README.md") << std::endl;
		system("pause");
	}
}
```