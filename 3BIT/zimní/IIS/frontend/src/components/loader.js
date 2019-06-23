import React from 'react';

const Loader = () => {
    return (
        <div className="loader layout__inner">
            <div className="lds-grid">
                <div></div><div></div><div></div><div></div><div></div><div></div><div></div><div></div><div></div>
            </div>
            <h1>Načítám obsah</h1>
        </div> 
    )
}

export default Loader;