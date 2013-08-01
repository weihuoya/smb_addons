#include "stdafx.h"
#include "weibo.h"
#include "vsm.h"


using namespace std;

void WeiboCluster::Save(const std::string& file)
{
	ofstream fout(file);

	fout<<"topics: "<<endl;
	copy(topics_.begin(), topics_.end(), ostream_iterator<string>(fout, "\n"));
	fout<<endl;

	fout<<"mentions: "<<endl;
	copy(mentions_.begin(), mentions_.end(), ostream_iterator<string>(fout, "\n"));

	fout<<endl;
	transform(terms_.begin(), terms_.end(), ostream_iterator<string>(fout, "\n"), [](const Term& term)->string {
		stringstream sout;
		sout<<get<0>(term.second)<<". "<<term.first<<" : "<<get<1>(term.second);
		return sout.str();
	});
}


void WeiboCluster::VectorSpaceModel(WeiboCluster::Collection& collection)
{
	float weight;
	size_t index = 0, count = 0;
	
	TermMap::iterator iter;
	Collection::iterator ii;
	vector<string>::const_iterator jj;

    //获取所有词语
	for(ii = collection.begin(); ii != collection.end(); ++ii)
	{
		const Weibo& doc = **ii;

		for_each( doc.tokens.begin(), doc.tokens.end(), 
			[this, &index](const string& token) {
			WeiboCluster::TermMap::iterator iter = this->terms_.find(token);
			if(iter != this->terms_.end()) get<1>(iter->second) += 1;
			else this->terms_.insert(make_pair(token, make_tuple(index++, 1)));
		});

		count += doc.tokens.size();

		this->topics_.insert(this->topics_.end(), doc.topics.begin(), doc.topics.end());
		this->mentions_.insert(this->mentions_.end(), doc.mentions.begin(), doc.mentions.end());
	}

    //计算每个词语的 TF-IDF 值
	for(ii = collection.begin(); ii != collection.end(); ++ii)
    {
        for(jj = (*ii)->tokens.begin(); jj != (*ii)->tokens.end(); ++jj)
        {
			iter = this->terms_.find(*jj);
			if(iter != this->terms_.end())
			{
				index = get<0>(iter->second);
				
				//weight = collection.size();
				//weight /= get<1>(iter->second);
				//weight = log(weight);

				weight = get<1>(iter->second);
				weight /= count;
				weight = weight * log(weight) + (1.0f - weight) * log(1.0f - weight);
				weight = -weight;

				weight *= TermFrequency(*ii, *jj);
#if (_MSC_VER > 1600)
				(*ii)->tfidf.emplace_back(index, weight);
#else
				(*ii)->tfidf.push_back(make_pair(index, weight));
#endif
			}
        }

		sort((*ii)->tfidf.begin(), (*ii)->tfidf.end(), 
			[](const pair<size_t, float>& lhs, const pair<size_t, float>& rhs)->bool {
			return lhs.first < rhs.first;
		});
    }
}

float WeiboCluster::TermFrequency(const WeiboCluster::Document& doc, const string& term)
{
	int count = 0;
	vector<string>::const_iterator iter;
	const vector<string>& tokens = doc->tokens;
	
	for(iter = tokens.begin(); iter != tokens.end(); ++iter)
	{
		if(*iter == term) count += 1;
	}

	return static_cast<float>(count) / static_cast<float>(tokens.size());
}

float WeiboCluster::InverseDocumentFrequency(const WeiboCluster::Collection& collection, const string& term)
{
	int count = 0;
	WeiboCluster::Collection::const_iterator ii;
	
	for(ii = collection.begin(); ii != collection.end(); ++ii)
	{
		const vector<string>& tokens = (*ii)->tokens;
		if( std::find(tokens.begin(), tokens.end(), term) != tokens.end() ) count += 1;
	}

	return log((float)collection.size() / (float)count);
}



float WeiboCluster::CosineSimilarity(const vector<pair<size_t, float>>& x, const vector<pair<size_t, float>>& y)
{
    float zz = DotProduct(x, y);
    float xx = sqrt( DotProduct(x, x) );
    float yy = sqrt( DotProduct(y, y) );

    if(xx == 0.0f || yy == 0.0f) return 0;
    return zz / (xx * yy);
}

float WeiboCluster::DotProduct(const vector<pair<size_t, float>>& x, const vector<pair<size_t, float>>& y)
{
	float result = 0.0f;
	vector<pair<size_t, float>>::const_iterator ii = x.begin(), jj = y.begin();

	while(ii != x.end() && jj != y.end())
	{
		if(ii->first < jj->first) ++ii;
		else if(ii->first > jj->first) ++jj;
		else
		{
			result += ii->second * jj->second;
			++ii;
			++jj;
		}
	}

    return result;
}


void WeiboCluster::DocumentCluster(WeiboCluster::Cluster& result, WeiboCluster::Collection& collection, size_t k)
{
	bool stop = false;
	size_t index = 0;
    vector<size_t> random(k);
    vector<size_t>::const_iterator iter;
	Collection::const_iterator ii;
    Cluster cluster, prev;

	if(k > collection.size()) k = collection.size();
	
	result.clear();
	result.resize(k);

	generate(random.begin(), random.end(), [&index]()->size_t { return index++; });
	random_shuffle(random.begin(), random.end());
	
	VectorSpaceModel(collection);

	for(iter = random.begin(); iter != random.end(); ++iter)
	{
#if (_MSC_VER > 1600)
		cluster.emplace_back(1, collection[*iter]);
#else
		cluster.push_back(Collection(1, collection[*iter]));
#endif
	}

	while(!stop)
	{
		prev = cluster;

		for(ii = collection.begin(); ii != collection.end(); ++ii)
		{
			index = FindClosestClusterCenter(cluster, *ii);
			result[index].push_back(*ii);
		}

		CalculateMeanPoints(result);
		cluster = result;
		stop = CheckStoppingCriteria(prev, cluster);
		if(!stop) result.resize( collection.size());
	}
}


int WeiboCluster::FindClosestClusterCenter(const WeiboCluster::Cluster& cluster, const WeiboCluster::Document& doc)
{
    size_t i;
    vector<float> measure(cluster.size());

    for(i = 0; i < cluster.size(); ++i)
    {
        measure[i] = CosineSimilarity(cluster[i][0]->tfidf, doc->tfidf);
    }

	return std::distance( measure.begin(), std::max_element(measure.begin(), measure.end()) );
}


bool WeiboCluster::CheckStoppingCriteria(WeiboCluster::Cluster& prev_cluster, WeiboCluster::Cluster& new_cluster)
{
	static int counter = 0;
	bool stop = true;
	size_t count = 0, index = 0, i;
	vector<size_t> change_index;
	
	counter += 1;
	if(counter > 11000) return stop;

	change_index.assign(new_cluster.size(), 0);

	while( index < new_cluster.size() )
	{
		if( new_cluster[index].size() != prev_cluster[index].size() && !new_cluster[index].empty() && !prev_cluster[index].empty())
		{
			vector< std::pair<size_t, float> >& new_tfidf  = new_cluster[index][0]->tfidf;
			vector< std::pair<size_t, float> >& prev_tfidf = prev_cluster[index][0]->tfidf;

			for(i = 0; i < new_tfidf.size(); ++i)
			{
				if(new_tfidf[i] == prev_tfidf[i]) ++count;
			}

			change_index[index] = count == new_tfidf.size() ? 0 : 1;
		}

		count = 0;
		index += 1;
	}

	if( change_index.end() != std::find(change_index.begin(), change_index.end(), 1) ) stop = false;

	return true;
}


//cluster 第一个 Weibo 是中心点，TF-IDF取平均值
void WeiboCluster::CalculateMeanPoints(WeiboCluster::Cluster& cluster)
{
	Cluster::iterator ii;
	Collection::iterator jj;
	vector<pair<size_t, float>>::const_iterator kk;
	map<size_t, float> sum;
	map<size_t, float>::iterator iter;

	for(ii = cluster.begin(); ii != cluster.end(); ++ii)
	{
		if(ii->empty()) continue;

		for(jj = ii->begin(); jj != ii->end(); ++jj)
		{
			vector< std::pair<size_t, float> >& tfidf = (*jj)->tfidf;

			for(kk = tfidf.begin(); kk != tfidf.end(); ++kk)
			{
				iter = sum.find(kk->first);
				if(iter == sum.end()) sum.insert(*kk);
				else iter->second += kk->second;
			}
		}

		jj = ii->begin();
		(*jj)->tfidf.clear();
		
		transform(sum.begin(), sum.end(), back_inserter((*jj)->tfidf), 
			[&ii](const pair<size_t, float>& x)->pair<size_t, float> {
				pair<size_t, float> y(x);
				y.second /= ii->size();
				return y;
		});

		sum.clear();
	}
}