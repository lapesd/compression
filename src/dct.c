/*
 * Copyright(C) 2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of GMPR.
 * 
 * GMPR is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * GMPR is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GMPR. If not, see <http://www.gnu.org/licenses/>.
 */

void dct(float **DCTMatrix, float **Matrix, int N, int M)
{
	for (int u = 0; u < N; ++u)
	{
		for (int v = 0; v < M; ++v)
		{
			DCTMatrix[u][v] = 0;
			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < M; j++)
				{
					DCTMatrix[u][v] += Matrix[i][j]
						*cos(M_PI/((float)N)*(i+1./2.)*u)
						*cos(M_PI/((float)M)*(j+1./2.)*v);
				}
			}
		}
	}
}

void idct(float **Matrix, float **DCTMatrix, int N, int M)
{
	for (int u = 0; u < N; ++u)
	{
		for (int v = 0; v < M; ++v)
		{
			Matrix[u][v] = 1/4.*DCTMatrix[0][0];
			for(int i = 1; i < N; i++)
				Matrix[u][v] += 1/2.*DCTMatrix[i][0];
			for(int j = 1; j < M; j++)
				Matrix[u][v] += 1/2.*DCTMatrix[0][j];

			for (int i = 1; i < N; i++)
			{
				for (int j = 1; j < M; j++)
				{
					Matrix[u][v] += DCTMatrix[i][j]
						*cos(M_PI/((float)N)*(u+1./2.)*i)
						*cos(M_PI/((float)M)*(v+1./2.)*j);
				}
			}

			Matrix[u][v] *= 2./((float)N)*2./((float)M);
		}
	}
}
