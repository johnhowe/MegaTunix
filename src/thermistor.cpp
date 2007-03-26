#include <iostream>
#include <math.h>
using namespace std;
#include <fstream>
#include "thermistor.h"



/*****************************************************************************
* Parameterized constructor for the thermistor class. This is the actual
* useful one, which sets up the value of the bias resistor, accepts the input
* temp-resistance pairs and generates the rest of the data.
******************************************************************************
*/
thermistor::thermistor(char scale, samples &input, int bias, int sensor, char *cmnt) {
	double temp_k;
	int temptemp;
	int denstemp;

	temp_scale = scale;
	bias_value = bias;
	set_coefficients(input);
	this->sensor = sensor;
	comment = cmnt;
	cout << "comment in thermistor: " << comment << endl;
	int i;
	float j;
	for (i = 1, j = 1.0; i < 255; ++i, j += 1) {
		entries[i].adc = i;
		entries[i].ohms = (int)((bias_value / (1 - j/255)) - bias_value);
		temp_k = t_of_r(entries[i].ohms);
		entries[i].temp_c = (int)(temp_k - C_TO_K);
		entries[i].temp_f = (int)((double)entries[i].temp_c * 9/5 + 32);

		if (sensor) { // CLT sensor
			temptemp = entries[i].temp_f + 40;
			if (temptemp < 0) { temptemp = 0;}
			if (temptemp > 255) {temptemp = 255; }
			entries[i].ms_val = temptemp;
		} else {	// IAT sensor
			denstemp = (int)(TEMP_K_100_DENSITY * 100 / temp_k);
			entries[i].ms_val = denstemp;
		}
	}

	if (!sensor) {
		// Both ends of scale get limp-home value, 100% density
		entries[0].ms_val = 100;
		entries[255].ms_val = 100;
	} else {
		// Both ends of scale get limp-home value,170 deg F
		entries[0].ms_val = 210;
		entries[255].ms_val = 210;
	}  
}


/****************************************************************************
* This is the copy constructor for the thermistor class, though it's doubtful
* it'll ever get used.	
****************************************************************************/
thermistor::thermistor(thermistor & other) {
	char *words;
	int len;
	this->temp_scale = other.temp_scale;
	this->CA = other.CA;
	this->CB = other.CB;
	this->CC = other.CC;
	this->bias_value = other.bias_value;
	len = strlen(other.comment);
	words = new char[len+1];
	strncpy(this->comment, other.comment, len+1);
	for (int i = 0; i < 256; ++i) {
		this->entries[i] = other.entries[i];
	}
	this->sensor = other.sensor;
}


/****************************************************************************
* Write this thermistor's inc file
****************************************************************************/
int thermistor::write_inc_file() {
	char filename[17];
	ofstream fout;
	if (sensor) { strncpy(filename, "thermfactor.inc\0", 16); }
	else { strncpy(filename, "airdenfactor.inc\0", 17); }
	cout << "Writing " << filename << endl;
	fout.open(filename);
	if (sensor) {
		fout << "; Generated by MegaTtunix integrated MultiTherm\n; ";
		fout << comment << endl << "; computed Steinhart-Hart coefficients:\n";
		fout << "; A=" << CA << "  B=" << CB << "  C=" << CC << endl;
		fout << "; For use with " << bias_value << " ohm resistor at R7\n";
		fout << "THERMFACTOR:\n;\t\tADC\tFahrenheit - Centigrade - resistance\n";
		fout << "\tDB\t210T;	   0 - sensor failure, use limp home value.\n";

		for (int i = 1; i < 255; ++i) {
			fout << "\tDB\t" << entries[i].ms_val << "T;\t" << i << "\t" 
				<< entries[i].temp_f << "\t" << entries[i].temp_c << "\t"
				<< entries[i].ohms << " ohms\n";
		}
		fout << "\tDB	210T;	 255 - sensor failure, use limp home value.\n";
	} else {
		fout << "; Generated by MegaTunix integrated MultiTherm\n; ";
		fout << comment << endl << "; computed Steinhart-Hart coefficients:\n"
			<< "; A=" << CA << "  B=" << CB << "  C=" << CC << endl
			<< "; For use with " << bias_value << " ohm resistor at R4\n"
			<< "AIRDENFACTOR:\n;\tADC\tFahrenheit - Centigrade - resistance\n"
			<< "\tDB	100T;	0 - sensor failure, use limp home value.\n";

		for (int i = 1; i < 255; ++i) {
			fout << "\tDB\t" << entries[i].ms_val << "T;\t" << i << "\t" 
				<< entries[i].temp_f << "\t" << entries[i].temp_c << "\t"
				<< entries[i].ohms << " ohms\n";
		}
		fout << "\tDB	100T;	255 - sensor failure, use limp home value.\n\n";
	}
	fout.close();
	return 0;
}


/****************************************************************************
* Modify the .s19 file specified by FILENAME. If this thermistor object is an
* IAT thermistor, it should modify the 256 entries starting at F500, if it is
* a CLT thermistor, it should modify those starting at F600.
****************************************************************************/
void thermistor::modify_s19_file(char filename[]) {
	char *mangled_name;
	char replace_locate[9];
	int start_addr;
	int end_addr;
	char line[44];
	unsigned char sum;
	int namelen, i, j, modified;

	namelen = strlen(filename + 1);
	mangled_name = new char[namelen + 4];
	strncpy(mangled_name, "temp", 4);
	strncpy(&mangled_name[4], filename, namelen);
	ifstream infile(filename);
	ofstream outfile(mangled_name);
	hex(outfile);
	outfile.setf(ios_base::uppercase);

	// Which chunk of s19 do we replace?
	if (sensor /*CLT*/) {
		strncpy(replace_locate, "S113F500\0", 9);
		start_addr = 0XF500;
		end_addr = 0XF600;
	} else /*IAT*/{
		strncpy(replace_locate, "S113F600\0", 9);
		start_addr = 0XF600;
		end_addr = 0XF700;
	}
	modified = 0;
	if (!infile.is_open()) {
		cout << "Error: Couldn't open " << filename << endl;
		exit(1);
	}
	while (infile.getline(line, 44)) {
		if (strncmp(line, replace_locate, 8) && !modified) {
			outfile << line << endl;
		} else if (!modified) {
			for (i = start_addr, j = 0; i < end_addr; i += 0X10) {
				outfile << "S113" << i;
				sum = 0x13;
				if (sensor) { sum += 0xF5; }
				else { sum += 0xF6; }
				sum += (i & 0xff); // just the low eight bits
				for(int k = j + 16; (j < k) && (j < 256); ++j) {
					outfile.width(2);
					outfile.fill('0');
					outfile << entries[j].ms_val;
					sum += entries[j].ms_val;
				}
				sum ^= 0xff;
				outfile << (int)sum << "\n";
			}
			++modified;
		} else {
			if (modified > 15) {
				outfile << line << endl;
			} else {
				++modified;
			}
		}
	}
	while (infile.getline(line, 44)) {
		for (i = 0; i < 10; ++i) {
			// ignore these lines, we just replaced them above 
		}
		outfile << line << endl;
	}

	infile.close();
	outfile.close();
	remove(filename);
	rename(mangled_name, filename);
}


/****************************************************************************
* Ragnarok. Er, no, actually just the destruction of this isolated object
****************************************************************************/
thermistor::~thermistor(void) {
	delete [] comment;
}


/***************************************************************************
* set_coefficients takes the temperature-resistance pairs found in the
* input_pairs structure "in" and generates the three Steinhart-Hart
* coefficients needed and stores them into the variables CA, CB and CC.
***************************************************************************/
void thermistor::set_coefficients(samples &in) {

	double c11,c12,c13,c21,c22,c23,c31,c32,c33;

	if (temp_scale == 'F') {
		in.t1 = (in.t1 - 32) * 5/9;
		in.t2 = (in.t2 - 32) * 5/9;
		in.t3 = (in.t3 - 32) * 5/9;
	}

	c11 = log(in.r1);
	c12 = pow(log(in.r1),3);
	c13 = 1/(in.t1 + C_TO_K);

	c21 = log(in.r2);
	c22 = pow(log(in.r2),3);
	c23 = 1/(in.t2 + C_TO_K);

	c31 = log(in.r3);
	c32 = pow(log(in.r3),3);
	c33 = 1/(in.t3 + C_TO_K);

	CC = ((c23-c13) - (c33-c13)*(c21-c11)/(c31-c11))/((c22-c12) - (c32-c12)*(c21-c11)/(c31-c11));

	CB = ((c33-c13) - (CC)*(c32-c12))/(c31-c11);

	CA = c13 - (CB)*c11 - (CC)*c12;  

	return;
}


/***************************************************************************
* t_of_r calculates the temperature Kelvin from an input in ohms and using
* the Steinhart-Hart coefficients stored in CA, CB and CC.
***************************************************************************/
int thermistor::t_of_r(int r) {

	double dr = (double)r;

	double t = 1 / (CA + CB*log(dr) + CC*pow(log(dr),3));

	return (int)t;
}
