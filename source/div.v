// clock divider
module div(clk, out);
	input clk;
	output out;
	reg out;
	reg count;

	initial //initialize to 0
	begin
		out = 0;
		count = 0;
	end

	always @(posedge clk)
	begin
		if(count == 1) //divide by 4
			out <= ~out;

		count <= ~count;
	end
endmodule
