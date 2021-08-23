faces=importdata('face.txt',' ');
vertices=importdata('vert.txt', ' ');

[E,PHI,L] = compute_eigens(vertices,faces);

