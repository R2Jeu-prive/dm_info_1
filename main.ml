type vector = float array;;
type 'a lenList = ('a list) * int;;


let print_vector (vect:vector) = 
  for i = 0 to ((Array.length vect)-1) do
    print_float vect.(i);
    print_string ", "
  done
;;

let print_vector_array (tab:vector array) =
  for i = 0 to ((Array.length tab)-1) do
    print_vector tab.(i);
    print_string "\n"
  done
;;

type kd_tree =
|Leaf
|Node of kd_tree * int * vector * kd_tree
;;

let f_cmp (i:int) =
  fun (x, y) -> x.(i) <= y.(i)
;;

let echanger (tab:vector array) (n:int) (m:int) = 
  let temp = tab.(n) in
    tab.(n) <- tab.(m); tab.(m) <- temp
;;

let partitionner (tab:vector array) (d:int) (f:int) (p:int) (cmp: vector*vector -> bool):int = 
  echanger tab f p;
  let nb_petits = ref 0 and nb_grands = ref 0 in
  while !nb_petits + !nb_grands < f-d do
    if(cmp (tab.(d + !nb_grands + !nb_petits), tab.(f))) then begin
      echanger tab (d + !nb_petits) (d + !nb_grands + !nb_petits);
      nb_petits := !nb_petits + 1
    end
    else
      nb_grands := !nb_grands + 1
  done;
  echanger tab (d + !nb_petits) f;
  d + !nb_petits
;;

let rec selection_rapide (tab:vector array) (d:int) (f:int) (r:int) (i:int): vector =
  if d < f then (
    let p = d + (Random.int (f-d+1)) in (
      let q = partitionner tab d f p (f_cmp i) in (
        if r = q then tab.(r) else (
          if r < q then selection_rapide tab d (q-1) r i else selection_rapide tab (q+1) f r i
        )
      )
    )
  ) else tab.(d)
;;


let squared_dist_uni_dim (a:vector) (b:vector) (i:int): float =
  (a.(i) -. b.(i))*.(a.(i) -. b.(i))
;;

let squared_dist (a:vector) (b:vector): float =
  let total = ref 0. in (
    for i = 0 to ((Array.length a)-1) do
      total := !total +. (squared_dist_uni_dim a b i)
    done;
    !total
  )
;;

let create_kd_tree (tab:vector array):kd_tree = 
  let rec k = Array.length tab.(0) and aux (d:int) (f:int) (i:int):kd_tree =
    if d > f then Leaf else (
      let split_index = d + (f-d)/2 in (
        let split_vector = selection_rapide tab d f split_index (i mod k) in (
          Node ((aux d (split_index-1) (i+1)), i mod k, split_vector, (aux (split_index+1) f (i+1)))
        )
      )
    )
  in aux 0 ((Array.length tab)-1) 0  
;;

let build_data (n: int) : vector array =
  Array.init n (fun i -> Array.init 2 (fun _ -> Random.float 1.))
;;

let pile (el:'a) (l:'a lenList) : 'a lenList = 
  let (realList, len) = l in (el::realList, len + 1)
;;

let rec nearest (n:int) (tree:kd_tree) (needle:vector): (vector*float) lenList =
  let rec try_add_to_ordered_bests (a:(vector*float) lenList) (new_best:vector*float) : (vector*float) lenList =
    let (_, new_d) = new_best
    and (bests, bests_length) = a
    in
    match bests with
    |[] -> ([new_best],1) (*adding cause best*)
    |(_,d)::_ when d < new_d && bests_length < n -> (new_best :: bests, bests_length + 1) (*adding cause worse but not full*)
    |(_,d)::_ when d < new_d -> a (*worse and full so nope*)
    |worse::tail when bests_length < n -> pile worse (try_add_to_ordered_bests (tail, bests_length-1) new_best) (*adding and keeping everyone*)
    |worse::tail -> try_add_to_ordered_bests (tail, bests_length-1) new_best (*adding and killing worse*)
  and get_worst_best_squared_dist (bests:(vector*float) lenList): float =
    match bests with
    |((_,d)::_,_) -> d
    |_ -> Float.infinity
  in
  (*let rec merge_bests (a:(vector*float) lenList) (b:(vector*float) lenList): (vector*float) lenList =
    match a with
    |(head::tail, len) -> merge_bests (tail, len-1) (try_add_to_ordered_bests b head)
    |_ -> b (*a is ([],0)*)
  in*)
  let rec work (current_tree:kd_tree) (bests:(vector*float) lenList) : (vector*float) lenList =
    match current_tree with
    |Leaf -> bests
    |Node(left, i, vect, right) -> (
      let squared_dist = squared_dist vect needle 
      and squared_dist_to_bar = squared_dist_uni_dim vect needle i
      and (_,bests_length) = bests
      in
      (*print_int bests_length;
      print_string "\n";*)
      if squared_dist_to_bar > get_worst_best_squared_dist bests && bests_length == n then (
        if (f_cmp i) (vect,needle) then work right bests else work left bests (*we chopped half of the tree search*)
      ) else (
        if (f_cmp i) (vect,needle)
        then work left (work right (try_add_to_ordered_bests bests (vect,squared_dist)))
        else work right (work left (try_add_to_ordered_bests bests (vect,squared_dist)))
      )
    )
  in
  work tree ([],0)
;;

let yellow = Graphics.rgb 210 160 4;;
let cx = 1000;;
let cy = 1000;;
let to_x pt = (pt *. (float_of_int cx)) |> int_of_float;;
let to_y pt = (pt *. (float_of_int cy)) |> int_of_float;;

let rec draw_kd_tree_aux (t: kd_tree) swx swy nex ney =
  match t with
  | Leaf -> ()
  | Node(g, dim, v_sep, d) ->
    begin
      let dx, dy = to_x v_sep.(0), to_y v_sep.(1) in
      if dim = 0 then
        begin
          Graphics.set_color Graphics.black;
          Graphics.moveto dx swy; Graphics.lineto dx ney;
          Graphics.set_color yellow;
          Graphics.fill_circle dx dy 5;
          draw_kd_tree_aux g swx swy dx ney;
          draw_kd_tree_aux d dx swy nex ney;
        end
      else
        begin
          Graphics.set_color Graphics.black;
          Graphics.moveto swx dy; Graphics.lineto nex dy;
          Graphics.set_color yellow;
          Graphics.fill_circle dx dy 5;
          draw_kd_tree_aux g swx swy nex dy;
          draw_kd_tree_aux d swx dy nex ney;
        end;
    end
;;

let draw_kd_tree (t: kd_tree) =
  (* Hyp : fenêtre graphique de taille cx * cy ouverte
     dessine l'arbre t
  *)
  draw_kd_tree_aux t 0 0 cx cy
;;

let rec show_bests (bests:(vector*float) lenList): unit = 
  match bests with
  |((vect,d)::tail, len) -> 
    print_vector vect;
    print_float d;
    print_string "\n\n";
    show_bests (tail, len-1)
  |_ -> ()
;;

let main () =
  Random.self_init ();
  let nb_points = 63 in
  let data = build_data nb_points in
  let kd_tree = (create_kd_tree data) in         (* TODO : remplacer ici par votre fonction de génération d'un arbre k dimensionel *)
  let bests = nearest 4 kd_tree [|0.2;0.2|] in
  show_bests bests;
  Graphics.open_graph " 1000x1000";
  draw_kd_tree kd_tree;
  let _ = Graphics.wait_next_event [Key_pressed] in
  Graphics.close_graph ();
;;

main ();;