import React from 'react';
import {Link} from "react-router-dom";

import placeholder from'../assets/black_12.jpg';

const Article = (payload) => {
	var data = payload.payload;
    var path = data.isCategory ? `${data.kategorie}/${data.druh_id}` : `caje/${data.kategorie}/${data.druh_id}`

    //console.log(`caje/${data.kategorie}/${data.druh_id}`)
	return (
		<article className={`article article--${data.kategorie}`}>
			<Link to={path}>
                <div className={`article__img article__img--${data.kategorie}`}>
                    <img src={placeholder} alt=""/>
                </div>
                <div className="article__inner">
                    <h2>{data.jmeno_caje}</h2>
                    <ul>
                        <li><span>Kvalita</span> {data.kvalita}</li>
                        <li><span>Louhovani</span> {data.louhavani}</li>
                    </ul>
                </div>
            </Link>
		</article>
	);
}

export default Article;