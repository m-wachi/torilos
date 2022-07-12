int main(int argc, int argv);

int myadd(int a, int b);


int myadd(int a, int b) {
	return a + b;
}


int main(int argc, int argv) {
	int a, b;
	a = 3;
	
	b = myadd(a, 4);
}

