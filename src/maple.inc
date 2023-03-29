# get_subs_rules_v := proc(ord)::list:
# 	local i,subs_rules:
# 	subs_rules := []:
# 	for i to nops(ord) do
# 	    subs_rules := [op(subs_rules),ord[i] = cat(v_, i)];
# 	end do:
# 	subs_rules;
# end proc:

get_subs_rules := proc(ord,to_v)::list:
	local i,subs_rules:
	subs_rules := []:
	if(to_v) then
		for i to nops(ord) do
		    subs_rules := [op(subs_rules), ord[i] = cat(v_, i)];
		end do:
	else
		for i to nops(ord) do
		    subs_rules := [op(subs_rules), cat(v_, i) = ord[i]];
		end do:
	end if:
	subs_rules;
end proc:

get_subs_rules_v := proc(ord)::list:           
	get_subs_rules(ord, true):  
end proc:

get_ord_v := proc(ord)::list:
	local subs_rules_v:
	subs_rules_v := get_subs_rules_v(ord):
	subs(subs_rules_v,ord):
end proc:

get_ord := proc(ord)::list:
	local subs_rules:
	subs_rules := get_subs_rules(ord, false):
	subs(subs_rules,ord):
end proc:


### 验证是否solution的解包含于result的零点中
### solution是maple求解的零点, result是求解的三角系统
### result的数据结构为 [[tf1],{us1}], [[tf2],{us2}],...; ord的数据结构为 [x1,x2,...]
with(ListTools):
with(SolveTools):

TriSeriesContain := proc(ps,ord,triser_result)::boolean:
	local i,solution,zero_test_series,zero_test_tf_series,zero_test_us_series,flag,num_solution,tf_series,index_of_zero_tf,us_series,us_with_zero_tf,product_us:
	# solution 是原方程组的零点
	solution := [PolynomialSystem(ps)]:
	# solution := solve(map(x -> x = 0, ps), ord):
	
	# zero_test_series 为各零点解分别带入于三角系统的结果
	zero_test_series := [seq(simplify(eval(triser_result, solution[i])), i = 1 .. nops(solution))]:
	zero_test_tf_series:= [seq(map(x -> x[1],zero_test_series[i]), i = 1 .. nops(zero_test_series))]:
	zero_test_us_series:= [seq(map(x -> x[2],zero_test_series[i]), i = 1 .. nops(zero_test_series))]:
	
	# 对每个solution(maple求得的解)判断零点是否在三角系统中
	flag := true:
	for num_solution from 1 by 1 to nops(solution) do
		#print(num_solution):
		# 将list元素变成set, 这样可以通过判断其中是否有{0}
		# 如果有{0},说明三角系统包含这个零点
		tf_series := map(x->convert(x,set),zero_test_tf_series[num_solution]):
		index_of_zero_tf := [SearchAll({0},tf_series)]:
	
		us_series := zero_test_us_series[num_solution]:
		us_with_zero_tf := [seq(us_series[index_of_zero_tf[i]], i = 1 .. nops(index_of_zero_tf))]:
		product_us := `*`@op:
		us_with_zero_tf := simplify(map(product_us, us_with_zero_tf)):
	
		
		#print("us_with_zero_tf", us_with_zero_tf, "index_of_zero_tf",index_of_zero_tf):
		
		if(Occurrences(0, us_with_zero_tf, (x, y) -> evalb(x <> y)) = 0) then
			flag := false:
		end if:
	end do:
	flag
end proc:

TriSeriesContained := proc(ps,ord,triser_result)::boolean:
	local num_system, i, tf, us, solution_tri, flag, value:

	flag := true:

	# 首先遍历每个三角系统
	for num_system from 1 by 1 to nops(triser_result) do
		# print(num_system):
		tf := triser_result[num_system][1]:
		us := triser_result[num_system][2]:
	
		# 得到当个三角系统的零点
		solution_tri := [PolynomialSystem(tf,us,engine = backsolve, preprocess = false)]:
	
		# 对每个零点带入原方程组PS中检验是否为原点
		for i from 1 by 1 to nops(solution_tri) do
			value := simplify(eval(ps, solution_tri[i])):
			if(convert(value,set) <> {0}) then
				flag := false:
			end if:
			# print(value):
		end do:
	end do:
	flag
end proc:

TriSeriesSolution := proc(ord,triser_result)::list:
	local num_system, tf, us, solution_tri, solution_tri_list:

	# solution_tri_list[i] 是第i个三角系统的零点
	solution_tri_list := []:

	# 首先遍历每个三角系统
	for num_system from 1 by 1 to nops(triser_result) do
		# print(num_system):
		tf := triser_result[num_system][1]:
		us := triser_result[num_system][2]:
	
		# 得到当个三角系统的零点
		solution_tri := [PolynomialSystem(tf,indets(tf),us)]:
		solution_tri_list := [op(solution_tri_list), solution_tri]:
	end do:
	solution_tri_list
end proc:

HasZeros := proc(ord,poly_sys):
	local poly_sys_sol, flag:
	flag := false;
	# poly_sys_sol := [PolynomialSystem(op(poly_sys)[1], ord, op(poly_sys)[2],tryhard = false)];
	poly_sys_sol := solve(map(x -> x = 0, poly_sys[1]) union map(x -> x <> 0, poly_sys[2]), ord);
	if(poly_sys_sol <> []) then
		flag := true:
	end if:
	flag:
end proc:


with(ConstructibleSetTools):
# HasZeros := proc(ord,poly_sys):	
# 	local R, flag:
# 	R := PolynomialRing(ord);
# 	flag := IsEmpty(map(x -> x = 0, poly_sys[1]) union map(x -> x <> 0, poly_sys[2]), R);
# 	not flag;
# end proc:


# 验证 Decom 实现的正确性
DecomTest := proc(ps,ord,tf_epsilon):
	local tf_set,my_tf_set,flag,i,j;

	if not type(ps[1],{set,list}) then
		tf_set := {op(map(factor,ps))} minus {0};
		tf_set := convert(`trisys/decom`(tf_set, ord),set);
	else
		tf_set := convert(`trisys/decom`(ps, ord),set);
	fi;
	
	my_tf_set := subs(get_subs_rules(ord,false), tf_epsilon);
	my_tf_set := convert(my_tf_set,set);
	
	tf_set := map(x -> map(y -> sign(y)*y, x), tf_set);
	my_tf_set := map(x -> map(y -> sign(y)*y, x), my_tf_set);

	flag := true;
	if (nops(tf_set) <> nops(my_tf_set)) then 
		flag := false;
	else
		for i from 1 by 1 to nops(tf_set) do
			# print(tf_set[i]);
			# print(my_tf_set[i]);
			if (nops(tf_set[i]) <> nops(my_tf_set[i])) then 
				flag := false;
				break;
			end if;
			for j from 1 by 1 to nops(tf_set[i]) do
				# print(tf_set[i]);
				# print(my_tf_set[i]);

				if(tf_set[i][j] <> my_tf_set[i][j] and tf_set[i][j] <> -my_tf_set[i][j]) then 
					# print(tf_set[i][j]);
					# print(my_tf_set[i][j]);
					flag := false;
					break;
				end if;
			end do;
			if(flag = false) then
				break;
			end if;
		end do;
	end if;
	flag
end proc: